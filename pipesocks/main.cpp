#include <QCoreApplication>
#include <QApplication>
#include "tcpserver.h"

int main(int argc,char **argv) {
    if (argc==1) {
        printf("If you want to use the CLI version\nUsage: %s [pump|pipe|tap|pac] <arguments>\n",*argv);
        QApplication a(argc,argv);
        return a.exec();
    } else {
        QCoreApplication a(argc,argv);
        QString type(a.arguments().at(1));
        if (type=="pump") {

        } else if (type=="pipe") {

        } else if (type=="tap") {

        } else if (type=="pac") {

        } else {
            printf("Usage: %s <pump|pipe|tap|pac> <arguments>\npump <bind port>\npipe [remote host] <remote port> <bind port>\ntap [remote host] <remote port> <local host> <bind port>\npac <bind port>\n",*argv);
            return 0;
        }
        return a.exec();
    }
}
