#include "pump.h"

Pump::Pump(qintptr handle,const QString &Password,QObject *parent):QObject(parent),Password(Password) {
    csock=new SecureSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(this);
    csock->moveToThread(cthread);
    cthread->start();
    ssock=NULL;
    sthread=NULL;
    usock=NULL;
    uthread=NULL;
    thread=new QThread(this);
    moveToThread(thread);
    thread->start();
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
    if (ssock) {
        ssock->disconnectFromHost();
        ssock->deleteLater();
        sthread->exit();
    }
    if (usock) {
        usock->close();
        usock->deleteLater();
        uthread->exit();
    }
    thread->exit();
    deleteLater();
}
