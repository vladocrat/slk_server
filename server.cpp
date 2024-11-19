#include "server.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QDataStream>
#include <QUuid>
#include <QDebug>

#include <vector>
#include <algorithm>

#include "room.h"
#include "protocol.h"
#include "messageparser.h"
#include "message.h"
#include "messagefactory.h"


namespace slk {

namespace
{

void send(QTcpSocket* client, std::unique_ptr<QByteArray>&& data)
{
    Q_ASSERT(client);

    QDataStream clientStream(client);
    clientStream << static_cast<uint64_t>(data->size()) << *data.get();
    client->flush(); //! TODO remove for packet optimization;
}

} //! Utils namespace

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
            const auto it = std::ranges::find(impl().pendingClients, newClient);
            
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
            auto [type, stream] = MessageParser::parseCommand(data);
            
            switch (type)
            {
            case Messages::MessageType::PING:
            {
                send(newClient, MessageFactory::create(Messages::MessageType::PING));
                break;
            }
            case Messages::MessageType::CONNECT_TO_ROOM:
            {
                QUuid roomId;
                MessageParser::parseData(stream, std::tie(roomId));
                qDebug() << roomId;
                
                const auto it = std::ranges::find_if(impl().rooms, [roomId](const auto& room)
                {
                    return room->id() == roomId;
                });
                
                if (it == impl().rooms.end())
                {
                    send(newClient, MessageFactory::create(Messages::MessageType::FAILED_TO_CONNECT_TO_ROOM, roomId));
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
                    send(newClient, MessageFactory::create(Messages::MessageType::FAILED_TO_CREATE_ROOM, QString("Room already exists")));
                    break;
                }

                qDebug() << "Created room with name: " << name;
                
                const auto room = std::make_shared<Room>();
                room->setName(name);
                impl().rooms.push_back(room);

                QObject::connect(room.get(), &Room::clientAdded, this, [](QTcpSocket* client, const QHostAddress& address, const uint32_t port) {
                    send(client, MessageFactory::create(Messages::MessageType::CONNECTED_TO_ROOM, address, port));
                });

                send(newClient, MessageFactory::create(Messages::MessageType::ROOM_CREATED, room->id()));
                break;
            }
            case Messages::MessageType::SEND_VOICE_MSG:
            case Messages::MessageType::FAILED_TO_CONNECT_TO_ROOM:
            case Messages::MessageType::ROOM_CREATED:
            case Messages::MessageType::FAILED_TO_CREATE_ROOM:
            case Messages::MessageType::CONNECTED_TO_ROOM:
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
