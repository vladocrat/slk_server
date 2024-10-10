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

using ParsingResult = std::pair<Messages::MessageType, QDataStream*>;

namespace
{

ParsingResult getMessageType(const QByteArray& data)
{
    QDataStream stream(data);
    uint32_t size;
    QByteArray payload;
    stream >> size;
    stream >> payload;
    
    QDataStream payloadStream(payload);
    Messages::MessageType type;
    payloadStream >> type;
    return std::make_pair(type, &payloadStream);
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
            QDataStream stream(data);
            uint32_t size;
            QByteArray payload;
            stream >> size;
            stream >> payload;
            
            QDataStream payloadStream(payload);
            Messages::MessageType type;
            payloadStream >> type;
            
            switch (type)
            {
            case Messages::MessageType::PING:
                MessageParser::Message msg;
                msg.type = Messages::MessageType::PING;
                msg.payload = {};
                
                QByteArray payload;
                QDataStream msgStream(&payload, QIODevice::WriteOnly);
                msgStream << msg.type << msg.payload;
                
                QDataStream clientStream(newClient);
                clientStream << sizeof(msg) << payload;
                newClient->flush();
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
