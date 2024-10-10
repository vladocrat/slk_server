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

void send(QTcpSocket* client, const MessageParser::Message& msg) noexcept
{
    QByteArray payload;
    
    {
        QDataStream msgStream(&payload, QIODevice::WriteOnly);
        msgStream << msg;
    }
    
    QDataStream clientStream(client);
    clientStream << sizeof(msg) << payload;
    client->flush(); //! TODO remove for packet optimization;
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
        
        QObject::connect(newClient, &QTcpSocket::readyRead, this, [newClient]()
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
