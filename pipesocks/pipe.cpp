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

#include "pipe.h"

Pipe::Pipe(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,QObject *parent):QObject(parent) {
    csock=new TcpSocket(this);
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    ssock=new TcpSocket(this);
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    CHost=csock->peerAddress();
    CPort=csock->peerPort();
    Log::log(csock,"connection established");
}

void Pipe::ClientRecv(const QByteArray &Data) {
    emit ssock->SendData(Data);
}

void Pipe::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pipe::EndSession() {
    if (csock->state()==QAbstractSocket::ConnectedState) {
        Log::log(CHost.toString().mid(7)+':'+QString::number(CPort)+" server closed the connection");
    }
    if (ssock->state()==QAbstractSocket::ConnectedState) {
        Log::log(CHost.toString().mid(7)+':'+QString::number(CPort)+" client closed the connection");
    }
    csock->disconnectFromHost();
    ssock->disconnectFromHost();
    if (ssock->state()==QAbstractSocket::UnconnectedState&&csock->state()==QAbstractSocket::UnconnectedState) {
        deleteLater();
    }
}
