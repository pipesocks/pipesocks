#include "pipe.h"

Pipe::Pipe(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(this);
    csock->moveToThread(cthread);
    cthread->start();
    ssock=new TcpSocket;
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    sthread=new QThread(this);
    ssock->moveToThread(sthread);
    sthread->start();
    thread=new QThread(this);
    moveToThread(thread);
    thread->start();
}

void Pipe::ClientRecv(const QByteArray &Data) {
    emit ssock->SendData(Data);
}

void Pipe::ServerRecv(const QByteArray &Data) {
    emit csock->SendData(Data);
}

void Pipe::EndSession() {
    csock->disconnectFromHost();
    csock->deleteLater();
    cthread->exit();
    cthread->deleteLater();
    ssock->disconnectFromHost();
    ssock->deleteLater();
    sthread->exit();
    sthread->deleteLater();
    thread->exit();
    thread->deleteLater();
    deleteLater();
}
