#include "server.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QDataStream>
#include <QUuid>
#include <QDebug>
#include <QCoreApplication>
#include <QSslConfiguration>
#include <QSslKey>

#include <vector>
#include <algorithm>

#include "room.h"
#include "protocol.h"
#include "messageparser.h"
#include "message.h"
#include "configurationcontroller.h"
#include "databasecontroller.h"
#include "userdata.h"
#include "hash.h"

#include <alog/logger.h>
DEFINE_ALOGGER_MODULE_NS(Server);

namespace slk {

namespace
{

void send(QTcpSocket* client, Message&& msg)
{
    Q_ASSERT(client);

    QByteArray payload;
    QDataStream payloadStream(&payload, QIODevice::WriteOnly);

    payloadStream << msg;

    bool t;
    QByteArray arr;

    QDataStream str(msg.payload);

    str >> t >> arr;
    LOGD << t << " " << QString(arr);

    QDataStream clientStream(client);
    clientStream << static_cast<uint64_t>(sizeof(payload)) << payload;
    client->flush(); //! TODO remove for packet optimization;
}

template<class... Args>
void send(QTcpSocket* client, const Args&... args)
{
    Q_ASSERT(client);

    QByteArray payload;
    QDataStream payloadStream(&payload, QIODevice::WriteOnly);

    ((payloadStream << args), ...);

    QDataStream clientStream(client);
    clientStream << static_cast<uint64_t>(sizeof(payload)) << payload;
    client->flush(); //! TODO remove for packet optimization;
}

} //! Utils namespace

struct Server::impl_t
{
    std::vector<QSslSocket*> pendingClients;
    std::vector<std::shared_ptr<Room>> rooms;
    DatabaseController dbController;
    QSslConfiguration sslConfiguration;
};

Server::Server()
{
    createImpl();
    
    const auto sslSettings = ConfigurationController::getSslSettings();

    if (!sslSettings) {
        qDebug() << "failed to get ssl settings";
        QCoreApplication::exit(-1);
    }

    const auto [certBinData, keyBinData] = sslSettings.value();

    impl().sslConfiguration = QSslConfiguration::defaultConfiguration();
    QSslCertificate sslCertificate(certBinData);
    QSslKey sslKey(keyBinData, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "server");
    impl().sslConfiguration.setPrivateKey(sslKey);
    impl().sslConfiguration.addCaCertificate(sslCertificate);
    impl().sslConfiguration.setLocalCertificate(sslCertificate);
    impl().sslConfiguration.setProtocol(QSsl::TlsV1_3);
    setSslConfiguration(impl().sslConfiguration);

    if (!impl().dbController.connect(ConfigurationController::getDBSettings())) {
        LOGE << "failed to connect to DB";
        QCoreApplication::exit(-1);
    }

    QObject::connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        bool ok = !impl().dbController.close();
        LOGD << (ok ? "connection closed" : "failed to close connection");
    });

    LOGI << "Connection to DB established succesfully";

    QObject::connect(this, &QSslServer::errorOccurred, this, [](QSslSocket*, QAbstractSocket::SocketError socketError) {
        LOGE << socketError;
    });
}

Server::~Server()
{
    for (auto client : impl().pendingClients)
    {
        client->close();
        client->deleteLater();
    }

    for (const auto& room : impl().rooms) {
        room->deleteLater();
    }

    impl().dbController.close();
}

void Server::readData(QSslSocket* const newClient, Messages::MessageType type, const std::shared_ptr<QDataStream>& stream)
{
    LOGD << static_cast<int>(type);

    switch (type)
    {
    case Messages::MessageType::PING:
    {
        send(newClient, Messages::MessageType::PING, QByteArray{});
        break;
    }
    case Messages::MessageType::CONNECT_TO_ROOM:
    {
        QUuid roomId;
        MessageParser::parseData(stream, std::tie(roomId));
        LOGD << roomId.toString();

        const auto it = std::ranges::find_if(impl().rooms, [roomId](const auto& room) {
                                                 return room->id() == roomId;
                                             });

        if (it == impl().rooms.end())
        {
            send(newClient, Messages::MessageType::FAILED_TO_CONNECT_TO_ROOM, roomId);
            break;
        }

        (*it)->addNewClient(newClient);

        break;
    }
    case Messages::MessageType::CREATE_ROOM:
    {
        //! TODO check from DB that the room with such name doesn't exist
        QString name;
        MessageParser::parseData(std::move(stream), std::tie(name));

        const auto found = std::ranges::find_if(impl().rooms, [&name](const auto& room) {
                               return room->name() == name;
                           }) != impl().rooms.end();

        if (found) {
            QString errorMsg = "Room already exists";
            send(newClient, Messages::MessageType::FAILED_TO_CREATE_ROOM, errorMsg.toUtf8());
            break;
        }

        LOGI << "Created room with name: " << name;

        const auto room = std::make_shared<Room>();
        room->setName(name);
        impl().rooms.push_back(room);

        QObject::connect(room.get(), &Room::clientAdded, this, [](QTcpSocket* client, const QHostAddress& address, const uint32_t port) {
            send(client, Messages::MessageType::CONNECTED_TO_ROOM, address, port);
        });

        send(newClient, Messages::MessageType::ROOM_CREATED, room->id());
        break;
    }
    case Messages::MessageType::LOG_IN:
    {
        QByteArray email;
        QByteArray password;
        MessageParser::parseData(std::move(stream), std::tie(email, password));

        LOGI << "Loging in user: Username(" << QString(email) << ")";

        if (!impl().dbController.logIn(email.toStdString(), createHash(password))) {
            LOGE << "Failed to login user";
            QString errorMsg = "Failed to login in";
            send(newClient, Messages::MessageType::LOG_IN, false, errorMsg.toUtf8());

            break;
        }

        LOGI << "User loged in successfully";
        send(newClient, Messages::MessageType::LOG_IN, true);
        break;
    }
    case Messages::MessageType::SEND_VOICE_MSG:
    case Messages::MessageType::FAILED_TO_CONNECT_TO_ROOM:
    case Messages::MessageType::ROOM_CREATED:
    case Messages::MessageType::FAILED_TO_CREATE_ROOM:
    case Messages::MessageType::CONNECTED_TO_ROOM:
        break;
    }
}

void Server::incomingConnection(qintptr handle)
{
    LOGI << "New connection!";

    const auto socket = new QSslSocket;
    socket->setSocketDescriptor(handle);
    socket->setSslConfiguration(impl().sslConfiguration);
    socket->startServerEncryption();

    QObject::connect(socket, &QSslSocket::alertReceived, this, [](QSsl::AlertLevel level, QSsl::AlertType type, const QString& description) {
        LOGD << level << " " << type << " " << description;
    });

    QObject::connect(socket, &QSslSocket::disconnected, this, [this, socket]() {
        LOGI << "Client disconnected";

        const auto it = std::ranges::find(impl().pendingClients, socket);

        if (it != impl().pendingClients.end()) {
            auto val = *it;
            impl().pendingClients.erase(it);
            val->deleteLater();
            val = nullptr;
        }
    });

    QObject::connect(socket, &QSslSocket::readyRead, this, [this, socket]() {
        //! TODO change to read it correctly (tcp packets)
        auto data = socket->readAll();
        auto [type, stream] = MessageParser::parseCommand(data);
        readData(socket, type, stream);
    });

    impl().pendingClients.push_back(socket);
}

} //! slk
