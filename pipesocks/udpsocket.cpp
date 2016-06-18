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

#include "udpsocket.h"

UdpSocket::UdpSocket(QObject *parent):QUdpSocket(parent) {
    connect(this,SIGNAL(SendData(QHostAddress,unsigned short,QByteArray)),SLOT(SendDataSlot(QHostAddress,unsigned short,QByteArray)));
    connect(this,SIGNAL(readyRead()),this,SLOT(RecvDataSlot()));
    connect(this,SIGNAL(Disconnect()),this,SLOT(DisconnectSlot()));
}

void UdpSocket::SendDataSlot(const QHostAddress &Host,unsigned short Port,const QByteArray &Data) {
    writeDatagram(Data,Host,Port);
}

void UdpSocket::RecvDataSlot() {
    while (hasPendingDatagrams()) {
        QByteArray Data;
        QHostAddress Host;
        unsigned short Port;
        Data.resize(pendingDatagramSize());
        readDatagram(Data.data(),Data.size(),&Host,&Port);
        emit RecvData(Host,Port,Data);
    }
}

void UdpSocket::DisconnectSlot() {
    close();
}
