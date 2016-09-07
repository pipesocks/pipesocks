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
    csock=new SecureSocket(this);
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    ssock=NULL;
    usock=NULL;
    status=Initiated;
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());
}

void Pump::ClientRecv(const QByteArray &Data) {
    QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap()),qvm2;
    switch (status) {
        case Initiated:
            if ((!Version::CheckVersion(qvm["version"].toString()))||qvm["password"]!=Password) {
                qvm2["status"]="refuse";
                emit csock->SendData(QJsonDocument::fromVariant(qvm2).toJson());
                csock->disconnectFromHost();
                break;
            }
            qvm2["status"]="ok";
            qvm2["protocol"]=qvm["protocol"];
            emit csock->SendData(QJsonDocument::fromVariant(qvm2).toJson());
            if (qvm["protocol"]=="tcp") {
                ssock=new TcpSocket(this);
                connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
                connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
                ssock->connectToHost(qvm["host"].toString(),qvm["port"].toUInt());
                status=TCP;
            } else if (qvm["protocol"]=="udp") {
                usock=new UdpSocket(this);
                connect(usock,SIGNAL(RecvData(QHostAddress,unsigned short,QByteArray)),this,SLOT(UDPRecv(QHostAddress,unsigned short,QByteArray)));
                usock->bind(0,QAbstractSocket::DontShareAddress);
                status=UDP;
            }
            break;
        case TCP:
            emit ssock->SendData(Data);
            break;
        case UDP:
            emit usock->SendData(qvm["host"].toString(),qvm["port"].toUInt(),QByteArray::fromBase64(qvm["data"].toByteArray()));
            break;
    }
}

void Pump::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pump::UDPRecv(const QHostAddress &Address,unsigned short Port,const QByteArray &Data) {
    QVariantMap qvm;
    qvm.insert("host",Address.toString());
    qvm.insert("port",Port);
    qvm.insert("data",Data.toBase64());
    emit csock->SendData(QJsonDocument::fromVariant(qvm).toJson());
}

void Pump::EndSession() {
    bool reset=csock->error()!=QAbstractSocket::RemoteHostClosedError;
    if (ssock) {
        reset&=ssock->error()!=QAbstractSocket::RemoteHostClosedError;
        if (reset)
            ssock->abort();
        else
            ssock->disconnectFromHost();
    }
    if (reset)
        csock->abort();
    else
        csock->disconnectFromHost();
    if (usock)
        usock->close();
    if (csock->state()==QAbstractSocket::UnconnectedState&&(ssock==NULL||ssock->state()==QAbstractSocket::UnconnectedState))
        deleteLater();
}
