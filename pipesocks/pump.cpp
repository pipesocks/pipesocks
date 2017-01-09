/*
This file is part of pipesocks. Pipesocks is a pipe-like SOCKS5 tunnel system.
Copyright (C) 2016  yvbbrjdr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pump.h"

Pump::Pump(qintptr handle,const QString &Password,QObject *parent):QObject(parent),Password(Password) {
    csock=new SecureSocket(Password,this);
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    ssock=NULL;
    usock=NULL;
    status=Initiated;
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());
}

void Pump::ClientRecv(const QByteArray &Data) {
    switch (status) {
        case Initiated: {
            QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap()),qvm2;
            if ((!Version::CheckVersion(qvm["version"].toString()))||qvm["password"]!=Password) {
                qvm2.insert("status","no");
                emit csock->SendData(QJsonDocument::fromVariant(qvm2).toJson());
                csock->disconnectFromHost();
                break;
            }
            if (qvm["protocol"]=="TCP") {
                ssock=new TcpSocket(this);
                connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
                connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
                ssock->connectToHost(qvm["host"].toString(),qvm["port"].toUInt());
                status=TCP;
            } else if (qvm["protocol"]=="UDP") {
                usock=new UdpSocket(this);
                connect(usock,SIGNAL(RecvData(QHostAddress,unsigned short,QByteArray)),this,SLOT(UDPRecv(QHostAddress,unsigned short,QByteArray)));
                status=UDP;
            }
            qvm2.insert("status","ok");
            qvm2.insert("protocol",qvm["protocol"].toString());
            emit csock->SendData(QJsonDocument::fromVariant(qvm2).toJson());
            break;
        }
        case TCP:
            emit ssock->SendData(Data);
            break;
        case UDP: {
            QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap());
            emit usock->SendData(qvm["host"].toString(),qvm["port"].toUInt(),QByteArray::fromBase64(qvm["data"].toByteArray()));
        }
    }
}

void Pump::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pump::EndSession() {
    if (ssock)
        ssock->disconnectFromHost();
    csock->disconnectFromHost();
    if (csock->state()==QAbstractSocket::UnconnectedState&&(ssock==NULL||ssock->state()==QAbstractSocket::UnconnectedState)) {
        if (usock)
            usock->close();
        deleteLater();
    }
}

void Pump::UDPRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data) {
    QVariantMap qvm;
    qvm.insert("host",Host.toString());
    qvm.insert("port",Port);
    qvm.insert("data",Data.toBase64());
    emit csock->SendData(QJsonDocument::fromVariant(qvm).toJson());
}
