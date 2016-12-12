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

#include "tap.h"

Tap::Tap(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,const QString &Password,QObject *parent):QObject(parent),Password(Password) {
    csock=new TcpSocket(this);
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    ssock=new SecureSocket(this);
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    status=Initiated;
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());
}

void Tap::ClientRecv(const QByteArray &Data) {
    switch (status) {
        case Initiated: {
            if (Data[0]!=5) {
                csock->disconnectFromHost();
                return;
            }
            bool ok=false;
            for (int i=2;i<Data[1]+2;++i) {
                if (Data[i]==0) {
                    ok=true;
                    break;
                }
            }
            if (!ok) {
                emit csock->SendData(QByteArray::fromHex("05ff"));
                csock->disconnectFromHost();
                return;
            }
            emit csock->SendData(QByteArray::fromHex("0500"));
            status=Handshook;
            break;
        }
        case Handshook: {
            QVariantMap qvm;
            QString host;
            unsigned short port;
            if (Data[0]!=5||Data[1]!=1||Data[2]!=0) {
                emit csock->SendData(QByteArray::fromHex("05070001000000000000"));
                csock->disconnectFromHost();
                return;
            }
            if (Data[3]==1) {
                host=QString("%1.%2.%3.%4")
                        .arg((unsigned char)Data[4])
                        .arg((unsigned char)Data[5])
                        .arg((unsigned char)Data[6])
                        .arg((unsigned char)Data[7]);
            } else if (Data[3]==3) {
                host=Data.mid(5,Data[4]);
            } else if (Data[3]==4) {
                host=QString("%1%2:%3%4:%5%6:%7%8:%9%10:%11%12:%13%14:%15%16")
                        .arg((unsigned char)Data[4],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[5],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[6],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[7],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[8],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[9],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[10],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[11],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[12],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[13],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[14],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[15],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[16],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[17],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[18],2,16,QLatin1Char('0'))
                        .arg((unsigned char)Data[19],2,16,QLatin1Char('0'));
            }
            qvm.insert("host",host);
            port=((unsigned char)Data[Data.length()-2]<<8)+(unsigned char)Data[Data.length()-1];
            qvm.insert("port",port);
            qvm.insert("password",Password);
            qvm.insert("version",Version::GetVersion());
            emit ssock->SendData(QJsonDocument::fromVariant(qvm).toJson());
            break;
        }
        case Connected:
            emit ssock->SendData(Data);
    }
}

void Tap::ServerRecv(const QByteArray &Data) {
    switch (status) {
        case Handshook: {
            QVariantMap qvm(QJsonDocument::fromJson(Data).toVariant().toMap());
            if (qvm["status"]=="ok") {
                emit csock->SendData(QByteArray::fromHex("05000001000000000000"));
                status=Connected;
            } else {
                emit csock->SendData(QByteArray::fromHex("05020001000000000000"));
                csock->disconnectFromHost();
            }
            break;
        }
        case Connected:
            emit csock->SendData(Data);
    }
}

void Tap::EndSession() {
    csock->disconnectFromHost();
    ssock->disconnectFromHost();
    if (ssock->state()==QAbstractSocket::UnconnectedState&&csock->state()==QAbstractSocket::UnconnectedState)
        deleteLater();
}

//QString("function FindPropxyForURL(url,host){return \"SOCKS5 %1;SOCKS %1\"}");
