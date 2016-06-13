#include "pump.h"

Pump::Pump(qintptr handle,const QString &Password,QObject *parent):QObject(parent),Password(Password) {
    csock=new SecureSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    csock->moveToThread(cthread);
    cthread->start();
    ssock=NULL;
    sthread=NULL;
    usock=NULL;
    uthread=NULL;
    status=Initiated;
}

void Pump::ClientRecv(const QByteArray &Data) {
    QString addr;
    unsigned short port;
    switch (status) {
        case Initiated:
            addr=Data.mid(5,Data[4]);
            port=((unsigned char)Data[Data.length()-2]<<8)+(unsigned char)Data[Data.length()-1];
            ssock=new TcpSocket;
            connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
            connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
            ssock->connectToHost(addr,port);
            sthread=new QThread(ssock);
            ssock->moveToThread(sthread);
            sthread->start();
            status=Connected;
            break;
        case Connected:
            emit ssock->SendData(Data);
            break;
    }
}

void Pump::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pump::UdpRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data) {

}

void Pump::EndSession() {
    emit csock->Disconnect();
    cthread->exit();
    cthread->wait();
    csock->deleteLater();
    if (ssock) {
        emit ssock->Disconnect();
        sthread->exit();
        sthread->wait();
        ssock->deleteLater();
    }
    if (usock) {
        uthread->exit();
        uthread->wait();
        usock->deleteLater();
    }
    deleteLater();
}
