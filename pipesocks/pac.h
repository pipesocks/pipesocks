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

#ifndef PAC_H
#define PAC_H

#include <QObject>
#include <QThread>
#include "tcpsocket.h"

class PAC : public QObject {
    Q_OBJECT
public:
    explicit PAC(qintptr handle,QObject *parent = 0);
private:
    TcpSocket *csock;
    QThread *cthread;
private slots:
    void RecvData(const QByteArray &Data);
    void EndSession();
};

#endif // PAC_H
