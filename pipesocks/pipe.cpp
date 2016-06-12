#include "pipe.h"

Pipe::Pipe(qintptr handle,const QString &RemoteHost,unsigned short RemotePort,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(ClientRecv(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread;
    csock->moveToThread(cthread);
    cthread->start();
    ssock=new TcpSocket;
    connect(ssock,SIGNAL(RecvData(QByteArray)),this,SLOT(ServerRecv(QByteArray)));
    connect(ssock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    ssock->connectToHost(RemoteHost,RemotePort);
    sthread=new QThread;
    ssock->moveToThread(sthread);
    sthread->start();
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
    cthread->wait();
    cthread->deleteLater();
    ssock->disconnectFromHost();
    ssock->deleteLater();
    sthread->exit();
    sthread->wait();
    sthread->deleteLater();
    deleteLater();
}
