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

#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include "tcpserver.h"
#include "mainform.h"

QString FindArg(const QStringList &Arguments,char Letter) {
    int index=Arguments.indexOf(QString('-')+Letter);
    if (index!=-1&&index<Arguments.size()-1)
        return Arguments.at(index+1);
    return QString();
}

int main(int argc,char **argv) {
    QString Usage(QString("Usage: %1 [pump|pipe|tap] <arguments>\nArguments:\n-H Remote Host\n-P Remote Port\n-p Local Port\n-k Password\n").arg(QString(*argv)));
    if (argc==1) {
        printf("%s",Usage.toStdString().c_str());
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QGuiApplication a(argc,argv);
        QQuickStyle::setStyle("Material");
        QQmlApplicationEngine engine;
        engine.load(QUrl(QLatin1String("qrc:/Main.qml")));
        new MainForm(engine.rootObjects().value(0));
        return a.exec();
    } else {
        QCoreApplication a(argc,argv);
        QStringList args=a.arguments();
        QString type=args.at(1),RemoteHost=FindArg(args,'H'),Password=FindArg(args,'k');
        unsigned short RemotePort=FindArg(args,'P').toUShort(),LocalPort=FindArg(args,'p').toUShort();
        RemotePort=(RemotePort==0)?7473:RemotePort;
        LocalPort=(LocalPort==0)?7473:LocalPort;
        TcpServer *server;
        if (type=="pump") {
            server=new TcpServer(TcpServer::PumpServer,RemoteHost,RemotePort,Password);
            printf("Welcome to Pipesocks pump\nServer is listening at port %d\n",LocalPort);
        } else if (type=="pipe") {
            if (RemoteHost=="") {
                printf("Remote Host required\n%s",Usage.toStdString().c_str());
                return 1;
            }
            server=new TcpServer(TcpServer::PipeServer,RemoteHost,RemotePort,Password);
            printf("Welcome to Pipesocks pipe\nServer is listening at port %d and connects to %s:%d\n",LocalPort,RemoteHost.toStdString().c_str(),RemotePort);
        } else if (type=="tap") {
            if (RemoteHost=="") {
                printf("Remote Host required\n%s",Usage.toStdString().c_str());
                return 1;
            }
            server=new TcpServer(TcpServer::TapClient,RemoteHost,RemotePort,Password);
            printf("Welcome to Pipesocks tap\nServer is listening at port %d and connects to %s:%d\n",LocalPort,RemoteHost.toStdString().c_str(),RemotePort);
        } else {
            printf("%s",Usage.toStdString().c_str());
            return 1;
        }
        if (!server->listen(QHostAddress::Any,LocalPort)) {
            printf("Failed to bind to port %d, exiting. . . \n",LocalPort);
            return 1;
        }
        return a.exec();
    }
}
