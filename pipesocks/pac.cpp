#include "pac.h"

PAC::PAC(qintptr handle,QObject *parent):QObject(parent) {
    csock=new TcpSocket;
    connect(csock,SIGNAL(RecvData(QByteArray)),this,SLOT(RecvData(QByteArray)));
    connect(csock,SIGNAL(disconnected()),this,SLOT(EndSession()));
    csock->setSocketDescriptor(handle);
    cthread=new QThread;
    csock->moveToThread(cthread);
    cthread->start();
}

void PAC::RecvData(const QByteArray &Data) {

}

void PAC::EndSession() {
    csock->disconnectFromHost();
    csock->deleteLater();
    cthread->exit();
    cthread->wait();
    cthread->deleteLater();
    deleteLater();
}
