#include "udpsocket.h"

UdpSocket::UdpSocket(QObject *parent):QUdpSocket(parent) {
    connect(this,SIGNAL(SendData(QString,unsigned short,QByteArray)),this,SLOT(SendDataSlot(QString,unsigned short,QByteArray)));
    connect(this,SIGNAL(readyRead()),this,SLOT(RecvDataSlot()));
}

void UdpSocket::SendDataSlot(const QString &Host,unsigned short Port,const QByteArray &Data) {
    QHostAddress address(Host);
    if (address.protocol()==-1) {
        QHostInfo info(QHostInfo::fromName(Host));
        if (info.addresses().size()==0)
            return;
        address=info.addresses().front();
    }
    writeDatagram(Data.data(),address,Port);
}

void UdpSocket::RecvDataSlot() {
    while (hasPendingDatagrams()) {
        QByteArray data(pendingDatagramSize(),0);
        QHostAddress address;
        quint16 port;
        readDatagram(data.data(),pendingDatagramSize(),&address,&port);
        emit RecvData(address,port,data);
    }
}
