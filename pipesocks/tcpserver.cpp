#include "tcpserver.h"

TcpServer::TcpServer(Mode mode,const QString &RemoteHost,unsigned short RemotePort,const QString &LocalHost,const QString &Password,QObject *parent):QTcpServer(parent),mode(mode),RemoteHost(RemoteHost),RemotePort(RemotePort),LocalHost(LocalHost),Password(Password) {}

void TcpServer::incomingConnection(qintptr handle) {
    if (mode==PumpServer) {
        Pump *pump=new Pump(handle,Password);
    } else if (mode==PipeServer) {
        Pipe *pipe=new Pipe(handle,RemoteHost,RemotePort);
    } else if (mode==TapClient) {
        Tap *tap=new Tap(handle,RemoteHost,RemotePort,LocalHost,Password);
    } else if (mode==PACServer) {
        PAC *pac=new PAC(handle);
    }
}
