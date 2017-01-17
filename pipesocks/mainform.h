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

#ifndef MAINFORM_H
#define MAINFORM_H

#include <QObject>
#include <QVariant>
#include <QSettings>
#include "log.h"
#include "version.h"
#include "tcpserver.h"

class MainForm : public QObject {
    Q_OBJECT
public:
    explicit MainForm(QObject *rootObject,QObject *parent = 0);
private:
    QObject *window,*headerText,*pipesocks,*about,
            *pump,*pipe,*tap,*remoteHost,
            *remotePort,*localPort,*password,*start,
            *dump,*info;
    void ShowError();
    TcpServer *server;
    QSettings *settings;
private slots:
    void pumpClicked();
    void pipeClicked();
    void tapClicked();
    void startClicked();
    void dumpClicked();
    void closing();
    void fileChosen(QUrl path);
};

#endif // MAINFORM_H
