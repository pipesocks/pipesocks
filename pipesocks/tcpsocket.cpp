#include "tcpsocket.h"

TcpSocket::TcpSocket(QObject *parent):QTcpSocket(parent) {
    connect(this,SIGNAL(SendData(QByteArray)),this,SLOT(SendDataSlot(QByteArray)));
    connect(this,SIGNAL(readyRead()),this,SLOT(RecvDataSlot()));
    connect(this,SIGNAL(Disconnect()),this,SLOT(DisconnectSlot()));
}

void TcpSocket::SendDataSlot(const QByteArray &Data) {
    write(Data);
}

void TcpSocket::RecvDataSlot() {
    emit RecvData(readAll());
}

void TcpSocket::DisconnectSlot() {
    disconnectFromHost();
}
