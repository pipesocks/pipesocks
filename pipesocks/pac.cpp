#include "pac.h"

PAC::PAC(qintptr handle,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(RecvData(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread(csock);
    csock->moveToThread(cthread);
    cthread->start();
}

void PAC::RecvData(const QByteArray &Data) {

}

void PAC::EndSession() {
    cthread->exit();
    cthread->wait();
    csock->deleteLater();
    deleteLater();
}
