#include <QCoreApplication>

#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    slk::Server* server = new slk::Server;
    server->listen(QHostAddress::LocalHost, 8084);
    
    return a.exec();
}
