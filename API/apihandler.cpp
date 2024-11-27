#include "apihandler.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtHttpServer/QHttpServer>
#include <QtHttpServer/QHttpServerResponse>
#include <QSslServer>
#include <QJsonObject>
#include <QSslKey>

#include <alog/logger.h>

#include "configurationcontroller.h"

DEFINE_ALOGGER_MODULE_NS(ApiHandler);

namespace slk
{

struct ApiHandler::impl_t
{
    QHttpServer server;
    std::unique_ptr<QSslServer> tcpServer;
};

ApiHandler::ApiHandler()
{
    createImpl();

    impl().server.route("/", this, [](QHttpServerResponder& responder) -> void {
        responder.write(QHttpServerResponder::StatusCode::Ok);
    });

    impl().server.route("/join_room/", this, []( const QString& request, QHttpServerResponder& responder) -> void {
        LOGD << request;

        const auto response = QHttpServerResponse(QJsonObject{
            {"message", "Hello, JSON!"}
        });

        LOGI << "Sending: " << response.data();
        responder.write(response.data(), response.mimeType());
    });
}

ApiHandler::~ApiHandler()
{

}

bool ApiHandler::initialize()
{
    impl().tcpServer = std::make_unique<QSslServer>();

    const auto settings = ConfigurationController::getSslSettings();

    if (!settings) {
        LOGE << "Failed to parse error";
        return false;
    }

    const auto [cert, key] = settings.value();

    auto sslConfiguration = QSslConfiguration::defaultConfiguration();
    QSslCertificate sslCertificate(cert);
    QSslKey sslKey(key, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.addCaCertificate(sslCertificate);
    sslConfiguration.setLocalCertificate(sslCertificate);
    sslConfiguration.setProtocol(QSsl::TlsV1_3);

    impl().tcpServer->setSslConfiguration(sslConfiguration);

    if (!impl().tcpServer->listen(QHostAddress::LocalHost, 65447) || !impl().server.bind(impl().tcpServer.get())) {
        impl().tcpServer->deleteLater();
        impl().tcpServer = nullptr;
        LOGE << "Failed to initialize http server";
        return false;
    }

    LOGI << "Http server started successfully on: "
         << impl().tcpServer->serverAddress() << ":" << impl().tcpServer->serverPort();

    return true;
}



} //! slk
