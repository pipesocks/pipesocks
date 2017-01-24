/*
This file is part of pipesocks. Pipesocks is a pipe-like SOCKS5 tunnel system.
Copyright (C) 2017  yvbbrjdr

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

#include "udpsocket.h"

UdpSocket::UdpSocket(QObject *parent):QUdpSocket(parent) {
    setProxy(QNetworkProxy::NoProxy);
    connect(this,SIGNAL(readyRead()),this,SLOT(RecvDataSlot()));
    connect(this,SIGNAL(SendData(QString,unsigned short,QByteArray)),this,SLOT(SendDataSlot(QString,unsigned short,QByteArray)));
    bind();
}

void UdpSocket::SendDataSlot(const QString &Host,unsigned short Port,const QByteArray &Data) {
    QHostAddress HostAddress(Host);
    if (HostAddress.protocol()==QAbstractSocket::UnknownNetworkLayerProtocol) {
        QHostInfo qhi(QHostInfo::fromName(Host));
        if (qhi.error()==QHostInfo::NoError) {
            HostAddress=qhi.addresses().first();
        } else {
            return;
        }
    }
    writeDatagram(Data,HostAddress,Port);
}

void UdpSocket::RecvDataSlot() {
    while (hasPendingDatagrams()) {
        QHostAddress Host;
        unsigned short Port;
        QByteArray Data(pendingDatagramSize(),0);
        readDatagram(Data.data(),pendingDatagramSize(),&Host,&Port);
        emit RecvData(Host,Port,Data);
    }
}
