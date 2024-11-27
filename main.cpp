#include <QCoreApplication>

#include <alog/logger.h>
#include <alog/sinks/file.h>

#include "api/apihandler.h"
#include "server/server.h"

DEFINE_ALOGGER_MODULE_NS(Main);

int main(int argc, char *argv[])
{
    ALog::DefaultLogger logger;
    //logger->setMode(ALog::Logger::LoggerMode::AsynchronousSort);
    logger->setAutoflush(false);
    logger->pipeline().sinks().add(std::make_shared<ALog::Sinks::File>("logs.txt"));
    logger.markReady();

    QCoreApplication a(argc, argv);

    slk::ApiHandler* handler = new slk::ApiHandler;
    handler->initialize();

    slk::Server* server = new slk::Server;
    server->listen(QHostAddress::LocalHost, 8084);

    LOGI << "Application started";
    
    return a.exec();
}
