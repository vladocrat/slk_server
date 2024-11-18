#include "server.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QDataStream>
#include <QUuid>

#include <vector>
#include <algorithm>

#include "room.h"
#include "protocol.h"
#include "messageparser.h"

#include <QDebug>

namespace slk {

namespace
{

template<class... Args>
std::unique_ptr<QByteArray> writeToByteArray(Args... arg)
{
    auto array = std::make_unique<QByteArray>();
    QDataStream stream(&(*array), QIODevice::WriteOnly);
    
    ((stream << arg), ...);
    return std::move(array);
}

void send(QTcpSocket* client, const MessageParser::Message& msg) noexcept
{
    if (!client) return;
    
    auto payload = writeToByteArray(msg);
    
    QDataStream clientStream(client);
    clientStream << sizeof(msg) << *payload;
    client->flush(); //! TODO remove for packet optimization;
}

template<class... Args>
void sendError(QTcpSocket* client, Messages::MessageType type, Args... arg)
{
    if (!client) return;
    
    MessageParser::Message msg;
    msg.type = type;
    
    auto payload = writeToByteArray(std::forward<Args...>(arg...));
    msg.payload = *payload;
    send(client, msg);
}

}

struct Server::impl_t
{
    std::vector<QTcpSocket*> pendingClients;
    std::vector<std::shared_ptr<Room>> rooms;
};

Server::Server()
{
    createImpl();
    
    QObject::connect(this, &QTcpServer::pendingConnectionAvailable, this, [this]()
    {
        const auto newClient = nextPendingConnection();
        impl().pendingClients.push_back(newClient);
        
        QObject::connect(newClient, &QTcpSocket::disconnected, this, [this, newClient]()
        {
            const auto it = std::find(impl().pendingClients.begin(), impl().pendingClients.end(), newClient);
            
            if (it != impl().pendingClients.end())
            {
                impl().pendingClients.erase(it);
                newClient->deleteLater();
            }
        });
        
        QObject::connect(newClient, &QTcpSocket::readyRead, this, [this, newClient]()
        {
            //! TODO change to read it correctly (tcp packets)
            auto data = newClient->readAll();
            const auto [type, stream] = MessageParser::parse(data);
            
            switch (type)
            {
            case Messages::MessageType::PING:
            {
                MessageParser::Message msg;
                msg.type = Messages::MessageType::PING;
                msg.payload = {};
                send(newClient, msg);
                break;
            }
            case Messages::MessageType::CONNECT_TO_ROOM:
            {
                QString id;
                *stream >> id;
                QUuid roomId(id);
                
                const auto it = std::ranges::find_if(impl().rooms, [roomId](const auto& room)
                {
                    return room->id() == roomId;
                });
                
                if (it != impl().rooms.end())
                {
                    sendError(newClient, Messages::MessageType::FAILED_TO_CONNECT_TO_ROOM, roomId);
                }
                
                (*it)->addNewClient(newClient);
                
                break;
            }
            case Messages::MessageType::CREATE_ROOM:
            {
                //! TODO check from DB that the room with such name doesn't exist
                QString name;
                *stream >> name;

                const auto found = std::ranges::find_if(impl().rooms, [&name](const auto& room) {
                    return room->name() == name;
                }) != impl().rooms.end();

                if (found) {
                    MessageParser::Message msg;
                    msg.type = Messages::MessageType::FAILED_TO_CREATE_ROOM;
                    auto payload = writeToByteArray(QString("Room already exists"));
                    msg.payload = *payload;

                    qDebug() << msg.payload;

                    send(newClient, msg);
                    break;
                }

                qDebug() << "Create room with name: " << name;
                
                const auto room = std::make_shared<Room>();
                room->setName(name);
                impl().rooms.push_back(room);

                MessageParser::Message msg;
                msg.type = Messages::MessageType::ROOM_CREATED;
                msg.payload = *writeToByteArray(room->id().toByteArray());
                qDebug() << msg.payload;

                send(newClient, msg);

                break;
            }
            case Messages::MessageType::FAILED_TO_CONNECT_TO_ROOM:
            case Messages::MessageType::ROOM_CREATED:
            case Messages::MessageType::FAILED_TO_CREATE_ROOM:
                break;
            }
        });
    });    
}

Server::~Server()
{
    for (auto client : impl().pendingClients)
    {
        client->close();
        client->deleteLater();
    }
}

} //! slk
