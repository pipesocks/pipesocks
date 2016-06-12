#include "pac.h"

PAC::PAC(qintptr handle,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(RecvData(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread;
    csock->moveToThread(cthread);
    cthread->start();
    thread=new QThread;
    moveToThread(thread);
    thread->start();
}

void PAC::RecvData(const QByteArray &Data) {

}

void PAC::EndSession() {
    csock->disconnectFromHost();
    csock->deleteLater();
    cthread->exit();
    cthread->deleteLater();
    thread->exit();
    thread->deleteLater();
    deleteLater();
}
