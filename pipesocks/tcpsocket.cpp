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

#include "tcpsocket.h"

TcpSocket::TcpSocket(QObject *parent):QTcpSocket(parent) {
    setProxy(QNetworkProxy::NoProxy);
    connect(this,SIGNAL(SendData(QByteArray)),this,SLOT(SendDataSlot(QByteArray)));
    connect(this,SIGNAL(readyRead()),this,SLOT(RecvDataSlot()));
}

void TcpSocket::SendDataSlot(const QByteArray &Data) {
    if (state()==QAbstractSocket::UnconnectedState)
        return;
    write(Data);
}

void TcpSocket::RecvDataSlot() {
    emit RecvData(readAll());
}
