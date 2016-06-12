#include "pump.h"

Pump::Pump(qintptr handle,const QString &Password,QObject *parent):QObject(parent),Password(Password) {
    csock=new SecureSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread;
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
    csock->disconnectFromHost();
    csock->deleteLater();
    cthread->exit();
    cthread->wait();
    cthread->deleteLater();
    if (ssock) {
        ssock->disconnectFromHost();
        ssock->deleteLater();
        sthread->exit();
        sthread->wait();
        sthread->deleteLater();
    }
    if (usock) {
        usock->close();
        usock->deleteLater();
        uthread->exit();
        uthread->wait();
        uthread->deleteLater();
    }
    deleteLater();
}
