#include "tcpsocket.h"

TcpSocket::TcpSocket(QObject *parent):QTcpSocket(parent) {
    connect(this,SIGNAL(SendData(QByteArray)),this,SLOT(SendDataSlot(QByteArray)));
    connect(this,SIGNAL(readyRead()),this,SLOT(RecvDataSlot()));
}

void TcpSocket::SendDataSlot(const QByteArray &Data) {
    write(Data);
}

void TcpSocket::RecvDataSlot() {
    emit RecvData(readAll());
}
