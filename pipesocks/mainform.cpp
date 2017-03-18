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

#include "mainform.h"

MainForm::MainForm(QObject *rootObject,QObject *parent):QObject(parent) {
    window=rootObject;
    headerText=window->findChild<QObject*>("headerText");
    pipesocks=window->findChild<QObject*>("pipesocks");
    about=window->findChild<QObject*>("about");
    pump=pipesocks->findChild<QObject*>("pump");
    pipe=pipesocks->findChild<QObject*>("pipe");
    tap=pipesocks->findChild<QObject*>("tap");
    remoteHost=pipesocks->findChild<QObject*>("remoteHost");
    remotePort=pipesocks->findChild<QObject*>("remotePort");
    localPort=pipesocks->findChild<QObject*>("localPort");
    password=pipesocks->findChild<QObject*>("password");
    start=pipesocks->findChild<QObject*>("start");
    dump=pipesocks->findChild<QObject*>("dump");
    info=about->findChild<QObject*>("info");
    server=NULL;
    headerText->setProperty("text","pipesocks "+Version::GetHighestVersion());
    info->setProperty("text","pipesocks "+Version::GetHighestVersion()+"\nCopyright (C) 2017  yvbbrjdr\nIcon by Rena\nQt by The Qt Company Ltd.\nlibsodium by jedisct1\nLicensed by GPL v3");
    connect(pump,SIGNAL(clicked()),this,SLOT(pumpClicked()));
    connect(pipe,SIGNAL(clicked()),this,SLOT(pipeClicked()));
    connect(tap,SIGNAL(clicked()),this,SLOT(tapClicked()));
    connect(start,SIGNAL(clicked()),this,SLOT(startClicked()));
    connect(dump,SIGNAL(clicked()),this,SLOT(dumpClicked()));
    connect(window,SIGNAL(closing(QQuickCloseEvent*)),this,SLOT(closing()));
    connect(window,SIGNAL(fileChosen(QUrl)),this,SLOT(fileChosen(QUrl)));
    connect(window,SIGNAL(windowStateChanged(Qt::WindowState)),this,SLOT(windowStateChanged(Qt::WindowState)));
    trayicon=new QSystemTrayIcon(this);
    trayicon->setIcon(QIcon(":/icons/win.ico"));
    trayicon->show();
    connect(trayicon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),window,SLOT(show()));
    connect(trayicon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),window,SLOT(requestActivate()));
    if (QSysInfo::macVersion()==QSysInfo::MV_None) {
        window->setProperty("color",QColor(48,48,48));
    }
    settings=new QSettings("yvbbrjdr","pipesocks",this);
    if (settings->contains("pipesocks/version")&&Version::CheckVersion(settings->value("pipesocks/version").toString())) {
        settings->beginGroup("default");
        QString type(settings->value("type").toString());
        if (type=="pump") {
            pump->setProperty("checked",true);
            QMetaObject::invokeMethod(pump,"clicked");
        } else if (type=="pipe") {
            pipe->setProperty("checked",true);
            QMetaObject::invokeMethod(pipe,"clicked");
        } else if (type=="tap") {
            tap->setProperty("checked",true);
            QMetaObject::invokeMethod(tap,"clicked");
        }
        remoteHost->setProperty("text",settings->value("remotehost").toString());
        remotePort->setProperty("text",settings->value("remoteport").toString());
        localPort->setProperty("text",settings->value("localport").toString());
        password->setProperty("text",settings->value("password").toString());
        settings->endGroup();
    } else {
        settings->clear();
    }
    QMetaObject::invokeMethod(remoteHost,"forceActiveFocus");
}

void MainForm::pumpClicked() {
    remoteHost->setProperty("enabled",false);
    remotePort->setProperty("enabled",false);
    localPort->setProperty("enabled",true);
    password->setProperty("enabled",true);
}

void MainForm::pipeClicked() {
    remoteHost->setProperty("enabled",true);
    remotePort->setProperty("enabled",true);
    localPort->setProperty("enabled",true);
    password->setProperty("enabled",false);
}

void MainForm::tapClicked() {
    remoteHost->setProperty("enabled",true);
    remotePort->setProperty("enabled",true);
    localPort->setProperty("enabled",true);
    password->setProperty("enabled",true);
}

void MainForm::startClicked() {
    if (start->property("text")=="Start") {
        if (pump->property("checked").toBool()) {
            if (localPort->property("text").toString()=="") {
                ShowError();
                return;
            }
            server=new TcpServer(TcpServer::PumpServer,remoteHost->property("text").toString(),remotePort->property("text").toString().toUShort(),password->property("text").toString(),this);
        } else if (pipe->property("checked").toBool()) {
            if (remoteHost->property("text").toString()==""||remotePort->property("text").toString()==""||localPort->property("text").toString()=="") {
                ShowError();
                return;
            }
            server=new TcpServer(TcpServer::PipeServer,remoteHost->property("text").toString(),remotePort->property("text").toString().toUShort(),password->property("text").toString(),this);
        } else if (tap->property("checked").toBool()) {
            if (remoteHost->property("text").toString()==""||remotePort->property("text").toString()==""||localPort->property("text").toString()=="") {
                ShowError();
                return;
            }
            server=new TcpServer(TcpServer::TapClient,remoteHost->property("text").toString(),remotePort->property("text").toString().toUShort(),password->property("text").toString(),this);
        }
        if (!server->listen(QHostAddress::Any,localPort->property("text").toString().toUInt())) {
            QMetaObject::invokeMethod(window,"showFailedBind");
            server->deleteLater();
            server=NULL;
            return;
        }
        pump->setProperty("enabled",false);
        pipe->setProperty("enabled",false);
        tap->setProperty("enabled",false);
        remoteHost->setProperty("enabled",false);
        remotePort->setProperty("enabled",false);
        localPort->setProperty("enabled",false);
        password->setProperty("enabled",false);
        start->setProperty("text","Stop");
        headerText->setProperty("text","Enjoy!");
        settings->setValue("pipesocks/version",Version::GetHighestVersion());
        settings->beginGroup("default");
        if (pump->property("checked").toBool()) {
            settings->setValue("type","pump");
        } else if (pipe->property("checked").toBool()) {
            settings->setValue("type","pipe");
        } else if (tap->property("checked").toBool()) {
            settings->setValue("type","tap");
        }
        settings->setValue("remotehost",remoteHost->property("text").toString());
        settings->setValue("remoteport",remotePort->property("text").toString());
        settings->setValue("localport",localPort->property("text").toString());
        settings->setValue("password",password->property("text").toString());
        settings->endGroup();
    } else if (start->property("text")=="Stop") {
        server->close();
        server->deleteLater();
        server=NULL;
        pump->setProperty("enabled",true);
        pipe->setProperty("enabled",true);
        tap->setProperty("enabled",true);
        if (pump->property("checked").toBool()) {
            pumpClicked();
        } else if (pipe->property("checked").toBool()) {
            pipeClicked();
        } else if (tap->property("checked").toBool()) {
            tapClicked();
        }
        start->setProperty("text","Start");
        headerText->setProperty("text","pipesocks "+Version::GetHighestVersion());
    }
}

void MainForm::dumpClicked() {
    if (dump->property("text")=="Dump") {
        QMetaObject::invokeMethod(window,"showFileDialog");
    } else if (dump->property("text")=="Undump") {
        Log::undump();
        dump->setProperty("text","Dump");
    }
}

void MainForm::closing() {
    if (server) {
        server->close();
        server->deleteLater();
    }
    trayicon->hide();
    QGuiApplication::exit();
}

void MainForm::ShowError() {
    QMetaObject::invokeMethod(window,"showNotFilled");
}

void MainForm::fileChosen(QUrl path) {
    Log::dump(path.toLocalFile());
    dump->setProperty("text","Undump");
}

void MainForm::windowStateChanged(Qt::WindowState state) {
    if (state==Qt::WindowMinimized&&QSysInfo::kernelType()!="linux") {
        QMetaObject::invokeMethod(window,"hide");
    }
}
