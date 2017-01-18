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

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include "pump.h"
#include "pipe.h"
#include "tap.h"
#include "gfwlist.h"

class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    enum Mode {
        PumpServer,
        PipeServer,
        TapClient
    };
    explicit TcpServer(Mode mode,const QString &RemoteHost,unsigned short RemotePort,const QString &Password,QObject *parent = 0);
private:
    Mode mode;
    QString RemoteHost;
    unsigned short RemotePort;
    QString Password;
    GFWList *gfwlist;
protected:
    void incomingConnection(qintptr handle);
};

#endif // TCPSERVER_H
