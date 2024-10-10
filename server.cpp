#include "server.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QDataStream>

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
QByteArray* writeToByteArray(Args... arg)
{
    QByteArray* array;
    QDataStream stream(array, QIODevice::WriteOnly);
    
    ((stream << arg), ...);
    return array;
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
    std::vector<Room> rooms;
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
                uint64_t roomId;
                (*stream.lock()) >> roomId;
                
                const auto it = std::find_if(impl().rooms.begin(), impl().rooms.end(), [roomId](const Room& room)
                {
                    return room.id() == roomId;
                });
                
                if (it != impl().rooms.end())
                {
                    sendError(newClient, Messages::MessageType::FAILED_TO_CONNECT_TO_ROOM, roomId);
                }
                
                it->addNewClient(newClient);
                
                break;
            }
            case Messages::MessageType::CREATE_ROOM:
            {
                //! TODO check from DB that the room with such name doesn't exist
                QString name;
                (*stream.lock()) >> name;
                
                Room room;
                room.setName(name);
                impl().rooms.push_back(room);
                
                QObject::connect(&room, &Room::clientAdded, this, [&room](QTcpSocket* client)
                {
                    MessageParser::Message msg;
                    msg.type = Messages::MessageType::ROOM_CREATED;
                    auto payload = writeToByteArray(room.id());
                    msg.payload = *payload;
                    
                    send(client, msg);
                });
                
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
