#include "tcpserver.h"

TcpServer::TcpServer(Mode mode,const QString &RemoteHost,unsigned short RemotePort,const QString &LocalHost,const QString &Password,QObject *parent):QTcpServer(parent),mode(mode),RemoteHost(RemoteHost),RemotePort(RemotePort),LocalHost(LocalHost),Password(Password) {}
