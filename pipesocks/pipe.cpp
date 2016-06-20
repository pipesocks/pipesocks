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

#include "pipe.h"

Pipe::Pipe(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    connect(csock,SIGNAL(disconnected()),cthread,SLOT(quit()));
    csock->moveToThread(cthread);
    cthread->start();
    ssock=new TcpSocket;
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    sthread=new QThread(ssock);
    connect(ssock,SIGNAL(disconnected()),sthread,SLOT(quit()));
    ssock->moveToThread(sthread);
    sthread->start();
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());
}

void Pipe::ClientRecv(const QByteArray &Data) {
    emit ssock->SendData(Data);
}

void Pipe::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pipe::EndSession() {
    if (csock) {
        disconnect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
        emit csock->Disconnect();
        cthread->wait();
        csock->deleteLater();
        csock=NULL;
    }
    if (ssock) {
        disconnect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
        emit ssock->Disconnect();
        sthread->wait();
        ssock->deleteLater();
        ssock=NULL;
    }
    deleteLater();
}
