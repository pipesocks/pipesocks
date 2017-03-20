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

#include "tcpserver.h"

TcpServer::TcpServer(Mode mode,const QString &RemoteHost,unsigned short RemotePort,const QString &Password,QObject *parent):QTcpServer(parent),mode(mode),RemoteHost(RemoteHost),RemotePort(RemotePort),Password(Password) {
    if (mode==TapClient)
        gfwlist=new GFWList(this);
    else
        gfwlist=NULL;
    setProxy(QNetworkProxy::NoProxy);
}

void TcpServer::incomingConnection(qintptr handle) {
    if (mode==PumpServer) {
        new Pump(handle,Password,this);
    } else if (mode==PipeServer) {
        new Pipe(handle,RemoteHost,RemotePort,this);
    } else {
        new Tap(handle,RemoteHost,RemotePort,Password,gfwlist,this);
    }
}
