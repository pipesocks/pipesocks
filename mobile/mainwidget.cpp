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

#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent):QWidget(parent),ui(new Ui::MainWidget) {
    ui->setupUi(this);
    TapServer=PACServer=NULL;
    ui->RemoteHost->setFocus();
    connect(ui->Start,SIGNAL(clicked(bool)),this,SLOT(StartClicked()));
}

MainWidget::~MainWidget() {
    delete ui;
}

void MainWidget::StartClicked() {
    if (ui->RemoteHost->text()==""||ui->RemotePort->text()==""||ui->LocalPort->text()=="") {
        QMessageBox::critical(this,"Error","Some blanks must be filled.");
        return;
    }
    TapServer=new TcpServer(TcpServer::TapClient,ui->RemoteHost->text(),ui->RemotePort->text().toUInt(),QString("127.0.0.1"),0,ui->Password->text(),this);
    TapServer->listen();
    PACServer=new TcpServer(TcpServer::PACServer,ui->RemoteHost->text(),ui->RemotePort->text().toUInt(),QString(),TapServer->serverPort(),ui->Password->text(),this);
    if (!PACServer->listen(QHostAddress::Any,ui->LocalPort->text().toUInt())) {
        QMessageBox::critical(this,"Error",QString("Failed to bind to port %1").arg(ui->LocalPort->text().toUInt()));
        TapServer->close();
        TapServer->deleteLater();
        PACServer->deleteLater();
        TapServer=PACServer=NULL;
        return;
    }
    ui->RemoteHost->setEnabled(false);
    ui->RemotePort->setEnabled(false);
    ui->LocalPort->setEnabled(false);
    ui->Password->setEnabled(false);
    ui->Start->setEnabled(false);
}

void MainWidget::closeEvent(QCloseEvent*) {
    if (TapServer) {
        TapServer->close();
        TapServer->deleteLater();
        PACServer->close();
        PACServer->deleteLater();
    }
}
