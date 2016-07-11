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
    setWindowOpacity(0.8);
    about=new AboutDialog(this);
    server=NULL;
    dragging=false;
    ui->RemoteHost->setFocus();
    connect(ui->Pump,SIGNAL(clicked(bool)),this,SLOT(PumpSelected()));
    connect(ui->Pipe,SIGNAL(clicked(bool)),this,SLOT(PipeSelected()));
    connect(ui->Tap,SIGNAL(clicked(bool)),this,SLOT(TapSelected()));
    connect(ui->PAC,SIGNAL(clicked(bool)),this,SLOT(PACSelected()));
    connect(ui->Pump,SIGNAL(clicked(bool)),this,SLOT(OtherSelected()));
    connect(ui->Pipe,SIGNAL(clicked(bool)),this,SLOT(OtherSelected()));
    connect(ui->Tap,SIGNAL(clicked(bool)),this,SLOT(OtherSelected()));
    connect(ui->Start,SIGNAL(clicked(bool)),this,SLOT(StartClicked()));
    connect(ui->About,SIGNAL(clicked(bool)),this,SLOT(AboutClicked()));
    if (QSysInfo::MacVersion()!=QSysInfo::MV_None)
        ui->PAC->setEnabled(false);
}

MainWidget::~MainWidget() {
    delete ui;
}

void MainWidget::PumpSelected() {
    ui->RemoteHost->setEnabled(false);
    ui->RemotePort->setEnabled(false);
    ui->LocalPort->setEnabled(true);
    ui->Password->setEnabled(true);
}

void MainWidget::PipeSelected() {
    ui->RemoteHost->setEnabled(true);
    ui->RemotePort->setEnabled(true);
    ui->LocalPort->setEnabled(true);
    ui->Password->setEnabled(false);
}

void MainWidget::TapSelected() {
    ui->RemoteHost->setEnabled(true);
    ui->RemotePort->setEnabled(true);
    ui->LocalPort->setEnabled(true);
    ui->Password->setEnabled(true);
}

void MainWidget::PACSelected() {
    ui->RemoteHost->setEnabled(false);
    ui->RemotePort->setEnabled(false);
    ui->LocalPort->setEnabled(true);
    ui->Password->setEnabled(false);
    if (ui->LocalPort->text()=="1080") {
        ui->LocalPort->setText("80");
    }
}

void MainWidget::OtherSelected() {
    if (ui->LocalPort->text()=="80") {
        ui->LocalPort->setText("1080");
    }
}

void MainWidget::StartClicked() {
    if (ui->Pump->isChecked()) {
        if (ui->LocalPort->text()=="") {
            QMessageBox::critical(this,"Error","Some blanks must be filled.");
            return;
        }
        server=new TcpServer(TcpServer::PumpServer,ui->RemoteHost->text(),ui->RemotePort->text().toUInt(),ui->Password->text(),this);
    } else if (ui->Pipe->isChecked()) {
        if (ui->RemoteHost->text()==""||ui->RemotePort->text()==""||ui->LocalPort->text()=="") {
            QMessageBox::critical(this,"Error","Some blanks must be filled.");
            return;
        }
        server=new TcpServer(TcpServer::PipeServer,ui->RemoteHost->text(),ui->RemotePort->text().toUInt(),ui->Password->text(),this);
    } else if (ui->Tap->isChecked()) {
        if (ui->RemoteHost->text()==""||ui->RemotePort->text()==""||ui->LocalPort->text()=="") {
            QMessageBox::critical(this,"Error","Some blanks must be filled.");
            return;
        }
        server=new TcpServer(TcpServer::TapClient,ui->RemoteHost->text(),ui->RemotePort->text().toUInt(),ui->Password->text(),this);
    } else if (ui->PAC->isChecked()) {
        if (ui->LocalPort->text()=="") {
            QMessageBox::critical(this,"Error","Some blanks must be filled.");
            return;
        }
        QFile file("proxy.pac");
        if (!file.exists()) {
            QMessageBox::critical(this,"Error","proxy.pac not exist.");
            return;
        }
        server=new TcpServer(TcpServer::PACServer,ui->RemoteHost->text(),ui->RemotePort->text().toUInt(),ui->Password->text(),this);
    }
    if (!server->listen(QHostAddress::Any,ui->LocalPort->text().toUInt())) {
        QMessageBox::critical(this,"Error",QString("Failed to bind to port %1").arg(ui->LocalPort->text().toUInt()));
        server->deleteLater();
        server=NULL;
        return;
    }
    ui->Pump->setEnabled(false);
    ui->Pipe->setEnabled(false);
    ui->Tap->setEnabled(false);
    ui->PAC->setEnabled(false);
    ui->RemoteHost->setEnabled(false);
    ui->RemotePort->setEnabled(false);
    ui->LocalPort->setEnabled(false);
    ui->Password->setEnabled(false);
    ui->Start->setEnabled(false);
}

void MainWidget::closeEvent(QCloseEvent*) {
    if (server) {
        server->close();
        server->deleteLater();
    }
}

void MainWidget::AboutClicked() {
    about->show();
}

void MainWidget::mousePressEvent(QMouseEvent *event) {
    dragging=true;
    oripos=event->pos();
}

void MainWidget::mouseMoveEvent(QMouseEvent *event) {
    if (dragging)
        move(pos()+event->pos()-oripos);
}

void MainWidget::mouseReleaseEvent(QMouseEvent*) {
    dragging=false;
}
