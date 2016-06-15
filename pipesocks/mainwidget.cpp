#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent):QWidget(parent),ui(new Ui::MainWidget) {
    ui->setupUi(this);
    ui->RemoteHost->setFocus();
}

MainWidget::~MainWidget() {
    delete ui;
}
