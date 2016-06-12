#include <QCoreApplication>
#include <QApplication>
#include "tcpserver.h"

QString FindArg(const QStringList &Arguments,char Letter) {
    int index=Arguments.indexOf(QString('-')+Letter);
    if (index!=-1&&index<Arguments.size()-1)
        return Arguments.at(index+1);
    return QString();
}

int main(int argc,char **argv) {
    if (argc==1) {
        printf("If you want to use the CLI version\nUsage: %s [pump|pipe|tap|pac] <arguments>\n",*argv);
        QApplication a(argc,argv);
        return a.exec();
    } else {
        QCoreApplication a(argc,argv);
        QStringList args=a.arguments();
        QString type=args.at(1),RemoteHost=FindArg(args,'H'),LocalHost=FindArg(args,'h'),Password=FindArg(args,'k');
        unsigned short RemotePort=FindArg(args,'P').toUInt(),LocalPort=FindArg(args,'p').toUInt();
        TcpServer *server;
        if (type=="pump") {
            if (LocalPort==0)
                LocalPort=1080;
            server=new TcpServer(TcpServer::Pump,RemoteHost,RemotePort,LocalHost,Password);
        } else if (type=="pipe") {
            if (RemoteHost=="") {
                printf("Pipesocks pipe needs Remote Host (-H Remote Host)");
                return 0;
            }
            if (RemotePort==0)
                RemotePort=1080;
            if (LocalPort==0)
                LocalPort=1080;
            server=new TcpServer(TcpServer::Pipe,RemoteHost,RemotePort,LocalHost,Password);
        } else if (type=="tap") {
            if (RemoteHost=="") {
                printf("Pipesocks tap needs Remote Host (-H Remote Host)");
                return 0;
            }
            if (LocalHost=="")
                LocalHost="127.0.0.1";
            if (RemotePort==0)
                RemotePort=1080;
            if (LocalPort==0)
                LocalPort=1080;
            server=new TcpServer(TcpServer::Tap,RemoteHost,RemotePort,LocalHost,Password);
        } else if (type=="pac") {
            if (LocalPort==0)
                LocalPort=80;
            server=new TcpServer(TcpServer::PAC,RemoteHost,RemotePort,LocalHost,Password);
        } else {
            printf("Usage: %s <pump|pipe|tap|pac> <arguments>\n\nArguments:\n\t-H Remote Host\n\t-P Remote Port <1080 by default>\n\t-h Local Host <127.0.0.1 by default>\n\t-p Local Port <80 for pac and 1080 for others by default>\n\t-k Password <empty by default>\n",*argv);
            return 0;
        }
        server->listen(QHostAddress::Any,LocalPort);
        return a.exec();
    }
}
