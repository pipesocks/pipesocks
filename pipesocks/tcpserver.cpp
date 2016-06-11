#include "tcpserver.h"

TcpServer::TcpServer(Mode mode,const QString &RemoteHost,unsigned short RemotePort,const QString &LocalHost,QObject *parent):QTcpServer(parent),mode(mode),RemoteHost(RemoteHost),RemotePort(RemotePort),LocalHost(LocalHost) {}
