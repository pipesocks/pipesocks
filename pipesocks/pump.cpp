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
    csock=new SecureSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    connect(csock,SIGNAL(disconnected()),cthread,SLOT(quit()));
    csock->moveToThread(cthread);
    cthread->start();
    ssock=NULL;
    sthread=NULL;
    usock=NULL;
    uthread=NULL;
    status=Initiated;
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());
}

void Pump::ClientRecv(const QByteArray &Data) {
    QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap());
    switch (status) {
        case Initiated:
            if (qvm["password"]!=Password) {
                emit csock->Disconnect();
            }
            if (qvm["protocol"]=="tcp") {
                ssock=new TcpSocket;
                connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
                connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
                ssock->connectToHost(qvm["host"].toString(),qvm["port"].toUInt());
                sthread=new QThread(ssock);
                connect(csock,SIGNAL(disconnected()),sthread,SLOT(quit()));
                ssock->moveToThread(sthread);
                sthread->start();
                status=TCP;
            } else if (qvm["protocol"]=="udp") {
                usock=new UdpSocket;
                connect(usock,SIGNAL(RecvData(QHostAddress,unsigned short,QByteArray)),this,SLOT(UdpRecv(QHostAddress,unsigned short,QByteArray)));
                uthread=new QThread(usock);
                usock->moveToThread(uthread);
                uthread->start();
                status=UDP;
            }
            break;
        case TCP:
            emit ssock->SendData(Data);
            break;
        case UDP:
            if (!qvm["host"].toString()[0].isNumber()) {
                QHostInfo host(QHostInfo::fromName(qvm["host"].toString()));
                if (host.addresses().empty()) {
                    emit csock->Disconnect();
                    return;
                }
                emit usock->SendData(host.addresses().front(),qvm["port"].toUInt(),qvm["data"].toByteArray());
            }
            emit usock->SendData(QHostAddress(qvm["host"].toString()),qvm["port"].toUInt(),qvm["data"].toByteArray());
            break;
    }
}

void Pump::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pump::UdpRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data) {
    QVariantMap qvm;
    qvm.insert("host",Host.toString());
    qvm.insert("port",Port);
    qvm.insert("data",Data);
    emit csock->SendData(QJsonDocument::fromVariant(qvm).toJson());
}

void Pump::EndSession() {
    if (csock) {
        emit csock->Disconnect();
        cthread->wait();
        csock->deleteLater();
        csock=NULL;
    }
    if (ssock) {
        emit ssock->Disconnect();
        sthread->wait();
        ssock->deleteLater();
        ssock=NULL;
    }
    if (usock) {
        emit usock->Disconnect();
        uthread->exit();
        uthread->wait();
        usock->deleteLater();
        usock=NULL;
    }
    deleteLater();
}
