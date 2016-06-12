#include "tap.h"

Tap::Tap(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,const QString &LocalHost,const QString &Password,QObject *parent):QObject(parent),Password(Password),LocalHost(LocalHost) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(this);
    csock->moveToThread(cthread);
    cthread->start();
    ssock=new SecureSocket;
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    sthread=new QThread(this);
    ssock->moveToThread(sthread);
    sthread->start();
    usock=NULL;
    uthread=NULL;
    thread=new QThread(this);
    moveToThread(thread);
    thread->start();
    status=Initiated;
}

void Tap::ClientRecv(const QByteArray &Data) {

}

void Tap::ServerRecv(const QByteArray &Data) {

}

void Tap::UdpRecv(const QHostAddress&,unsigned short,const QByteArray &Data) {

}

void Tap::EndSession() {
    csock->disconnectFromHost();
    csock->deleteLater();
    cthread->exit();
    ssock->disconnectFromHost();
    ssock->deleteLater();
    sthread->exit();
    if (usock) {
        usock->close();
        usock->deleteLater();
        uthread->exit();
    }
    thread->exit();
    deleteLater();
}
