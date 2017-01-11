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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QSystemTrayIcon>
#include "version.h"
#include "aboutdialog.h"
#include "tcpserver.h"
#include "log.h"

namespace Ui {
    class MainWidget;
}

class MainWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
private:
    Ui::MainWidget *ui;
    TcpServer *server;
    AboutDialog *about;
    bool dragging;
    QPoint oripos;
    QSettings *settings;
    QSystemTrayIcon *trayicon;
    void ShowError();
private slots:
    void PumpSelected();
    void PipeSelected();
    void TapSelected();
    void StartClicked();
    void AboutClicked();
    void DumpClicked();
    void Restore();
protected:
    void closeEvent(QCloseEvent*);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent*);
    void changeEvent(QEvent *event);
};

#endif // MAINWIDGET_H
