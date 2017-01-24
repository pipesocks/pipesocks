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

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QNetworkProxy>

class TcpSocket : public QTcpSocket {
    Q_OBJECT
public:
    explicit TcpSocket(QObject *parent = 0);
signals:
    void SendData(const QByteArray &Data);
    void RecvData(const QByteArray &Data);
private slots:
    virtual void SendDataSlot(const QByteArray &Data);
    virtual void RecvDataSlot();
};

#endif // TCPSOCKET_H
