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

#ifndef PIPE_H
#define PIPE_H

#include <QObject>
#include <QDateTime>
#include <QHostAddress>
#include "tcpsocket.h"
#include "log.h"

class Pipe : public QObject {
    Q_OBJECT
public:
    explicit Pipe(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,QObject *parent = 0);
private:
    TcpSocket *csock,*ssock;
    QHostAddress CHost;
    unsigned short CPort;
private slots:
    void ClientRecv(const QByteArray &Data);
    void ServerRecv(const QByteArray &Data);
    void EndSession();
};

#endif // PIPE_H
