#include "pac.h"

PAC::PAC(qintptr handle,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(RecvData(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(this);
    csock->moveToThread(cthread);
    cthread->start();
    thread=new QThread(this);
    moveToThread(thread);
    thread->start();
}

void PAC::RecvData(const QByteArray &Data) {

}

void PAC::EndSession() {
    csock->disconnectFromHost();
    csock->deleteLater();
    cthread->exit();
    thread->exit();
    deleteLater();
}
