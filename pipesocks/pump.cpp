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

}

void Pump::ServerRecv(const QByteArray &Data) {

}

void Pump::UdpRecv(const QHostAddress &Host,unsigned short Port,const QByteArray &Data) {

}

void Pump::EndSession() {
    cthread->exit();
    cthread->wait();
    csock->deleteLater();
    if (ssock) {
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
