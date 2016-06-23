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

#include "pac.h"

PAC::PAC(qintptr handle,unsigned short Port,QObject *parent):QObject(parent),Port(Port) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(RecvData(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    connect(csock,SIGNAL(disconnected()),cthread,SLOT(quit()));
    csock->moveToThread(cthread);
    cthread->start();
}

void PAC::RecvData(const QByteArray&) {
    QString pac=QString("function FindProxyForURL(url,host){var server=\"127.0.0.1:%1\";return \"SOCKS5 \"+server+\";SOCKS \"+server+\";DIRECT\";}").arg(Port);
    emit csock->SendData(QString("HTTP/1.1 200 OK\r\nContent-Type: application/x-ns-proxy-autoconfig\r\nConnection: close\r\nContent-Length: %1\r\n\r\n%2").arg(pac.length()).arg(pac).toLocal8Bit());
    emit csock->Disconnect();
}

void PAC::EndSession() {
    disconnect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(RecvData(QByteArray)));
    cthread->wait();
    csock->deleteLater();
    deleteLater();
}
