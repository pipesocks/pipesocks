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
    trayicon=new QSystemTrayIcon(this);
    if (QSysInfo::macVersion()!=QSysInfo::MV_None)
        trayicon->setIcon(QIcon(QApplication::applicationDirPath()+"/../Resources/osx.icns"));
    else
        trayicon->setIcon(QIcon(QApplication::applicationDirPath()+"/win.ico"));
    connect(trayicon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(Restore()));
    ui->RemoteHost->setFocus();
    connect(ui->Pump,SIGNAL(clicked(bool)),this,SLOT(PumpSelected()));
    connect(ui->Pipe,SIGNAL(clicked(bool)),this,SLOT(PipeSelected()));
    connect(ui->Tap,SIGNAL(clicked(bool)),this,SLOT(TapSelected()));
    connect(ui->Start,SIGNAL(clicked(bool)),this,SLOT(StartClicked()));
    connect(ui->About,SIGNAL(clicked(bool)),this,SLOT(AboutClicked()));
    connect(ui->Dump,SIGNAL(clicked(bool)),this,SLOT(DumpClicked()));
    settings=new QSettings("yvbbrjdr","pipesocks",this);
    if (settings->contains("pipesocks/version")&&Version::CheckVersion(settings->value("pipesocks/version").toString())) {
        settings->beginGroup("default");
        QString type(settings->value("type").toString());
        if (type=="pump") {
            ui->Pump->click();
        } else if (type=="pipe") {
            ui->Pipe->click();
        } else if (type=="tap") {
            ui->Tap->click();
        }
        ui->RemoteHost->setText(settings->value("remotehost").toString());
        ui->RemotePort->setText(settings->value("remoteport").toString());
        ui->LocalPort->setText(settings->value("localport").toString());
        ui->Password->setText(settings->value("password").toString());
        settings->endGroup();
        ui->Start->setFocus();
    } else {
        settings->clear();
    }
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

void MainWidget::ShowError() {
    QMessageBox::critical(this,"Error","Blanks must be filled.");
}

void MainWidget::StartClicked() {
    if (ui->Pump->isChecked()) {
        if (ui->LocalPort->text()=="") {
            ShowError();
            return;
        }
        server=new TcpServer(TcpServer::PumpServer,ui->RemoteHost->text(),ui->RemotePort->text().toUShort(),ui->Password->text(),this);
    } else if (ui->Pipe->isChecked()) {
        if (ui->RemoteHost->text()==""||ui->RemotePort->text()==""||ui->LocalPort->text()=="") {
            ShowError();
            return;
        }
        server=new TcpServer(TcpServer::PipeServer,ui->RemoteHost->text(),ui->RemotePort->text().toUShort(),ui->Password->text(),this);
    } else if (ui->Tap->isChecked()) {
        if (ui->RemoteHost->text()==""||ui->RemotePort->text()==""||ui->LocalPort->text()=="") {
            ShowError();
            return;
        }
        server=new TcpServer(TcpServer::TapClient,ui->RemoteHost->text(),ui->RemotePort->text().toUShort(),ui->Password->text(),this);
    }
    if (!server->listen(QHostAddress::Any,ui->LocalPort->text().toUInt())) {
        QMessageBox::critical(this,"Error",QString("Failed to bind to port %1").arg(ui->LocalPort->text().toUShort()));
        server->deleteLater();
        server=NULL;
        return;
    }
    ui->Pump->setEnabled(false);
    ui->Pipe->setEnabled(false);
    ui->Tap->setEnabled(false);
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
    settings->setValue("pipesocks/version",Version::GetVersion());
    settings->beginGroup("default");
    if (ui->Pump->isChecked()) {
        settings->setValue("type","pump");
    } else if (ui->Pipe->isChecked()) {
        settings->setValue("type","pipe");
    } else if (ui->Tap->isChecked()) {
        settings->setValue("type","tap");
    }
    settings->setValue("remotehost",ui->RemoteHost->text());
    settings->setValue("remoteport",ui->RemotePort->text());
    settings->setValue("localport",ui->LocalPort->text());
    settings->setValue("password",ui->Password->text());
    settings->endGroup();
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

void MainWidget::DumpClicked() {
    QString path(QFileDialog::getSaveFileName(this,"Where to dump?",QString(),"Log (*.log)"));
    if (path!="") {
        Log::dump(path);
        ui->Dump->setEnabled(false);
    }
}

void MainWidget::changeEvent(QEvent *event) {
    if (event->type()==QEvent::WindowStateChange&&windowState()&Qt::WindowMinimized) {
        trayicon->show();
        hide();
        trayicon->showMessage("Pipesocks is working on the background","Click the icon to restore");
    }
}

void MainWidget::Restore() {
    show();
    setWindowState(Qt::WindowNoState);
    trayicon->setVisible(false);
}
