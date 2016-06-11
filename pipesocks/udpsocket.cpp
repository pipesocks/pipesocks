#include "udpsocket.h"

UdpSocket::UdpSocket(QObject *parent):QUdpSocket(parent) {
    connect(this,SIGNAL(SendData(QHostAddress,unsigned short,QByteArray)),SLOT(SendDataSlot(QHostAddress,unsigned short,QByteArray)));
    connect(this,SIGNAL(readyRead()),this,SLOT(RecvDataSlot()));
}

void UdpSocket::SendDataSlot(const QHostAddress Host,unsigned short Port,const QByteArray &Data) {
    writeDatagram(Data,Host,Port);
}

void UdpSocket::RecvDataSlot() {
    while (hasPendingDatagrams()) {
        QByteArray Data;
        QHostAddress Host;
        unsigned short Port;
        Data.resize(pendingDatagramSize());
        readDatagram(Data.data(),Data.size(),&Host,&Port);
        emit RecvData(Host,Port,Data);
    }
}
