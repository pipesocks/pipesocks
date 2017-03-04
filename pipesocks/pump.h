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

#ifndef PUMP_H
#define PUMP_H

#include <QObject>
#include <QJsonDocument>
#include <QVariantMap>
#include <QDateTime>
#include <sodium.h>
#include "securesocket.h"
#include "tcpsocket.h"
#include "udpsocket.h"
#include "version.h"
#include "log.h"

class Pump : public QObject {
    Q_OBJECT
public:
    explicit Pump(qintptr handle,const QString &Password,QObject *parent = 0);
private:
    enum Status {
        Initiated,
        TCP,
        UDP
    };
    QString Password;
    Status status;
    SecureSocket *csock;
    TcpSocket *ssock;
    UdpSocket *usock;
    QHostAddress CHost;
    unsigned short CPort;
private slots:
    void ClientRecv(const QByteArray &Data);
    void ServerRecv(const QByteArray &Data);
    void UDPRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data);
    void EndSession();
};

#endif // PUMP_H
