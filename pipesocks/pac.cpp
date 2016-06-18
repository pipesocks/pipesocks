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

PAC::PAC(qintptr handle,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(RecvData(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    connect(csock,SIGNAL(disconnected()),cthread,SLOT(quit()));
    csock->moveToThread(cthread);
    cthread->start();
    printf("[%s] New connection from %s:%d\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data(),csock->peerAddress().toString().toLocal8Bit().data(),csock->peerPort());

}

void PAC::RecvData(const QByteArray&) {
    QFile pacfile("proxy.pac");
    if (!pacfile.open(QIODevice::Text|QIODevice::ReadOnly)) {
        printf("[%s] proxy.pac not readable\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data());
        return;
    }
    QTextStream qts(&pacfile);
    QString pac(qts.readAll());
    emit csock->SendData(QString("HTTP/1.1 200 OK\r\nContent-Type: application/x-ns-proxy-autoconfig\r\nConnection: close\r\nContent-Length: %1\r\n\r\n%2").arg(pac.length()).arg(pac).toLocal8Bit());
    emit csock->Disconnect();
}

void PAC::EndSession() {
    cthread->wait();
    csock->deleteLater();
    deleteLater();
}
