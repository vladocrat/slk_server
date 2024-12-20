#include "room.h"

#include <QString>
#include <QUuid>
#include <QDebug>
#include <QSslSocket>

#include <vector>

#include <alog/logger.h>

#include "client.h"

DEFINE_ALOGGER_MODULE_NS(Room);

namespace slk {

struct Room::impl_t
{
    QUuid id;
    QString name;
    std::vector<std::shared_ptr<Client>> clients;
};

Room::Room()
{
    createImpl();
    impl().id = QUuid::createUuid();
    LOGD << "new room: " << impl().id.toString();
}

Room::~Room()
{
    
}

QUuid Room::id() const noexcept
{
    return impl().id;
}

QString Room::name() const noexcept
{
    return impl().name;
}

void Room::setName(const QString& name) noexcept
{
    if (impl().name == name) return;
    
    impl().name = name;
}

void Room::addNewClient(QSslSocket* newClient) noexcept
{
    if (!newClient) return;
    
   auto client = std::make_shared<Client>(newClient);
    
    if (clientExists(client))
    {
        emit clientAlreadyExists(newClient);
        return;
    }

    const auto [address, port] = client->udpSettings();
    impl().clients.push_back(client);

    QObject::connect(client->tcpConnection(), &QTcpSocket::disconnected, this, [this, client]() {
        const auto [first, last] = std::ranges::remove_if(impl().clients, [&first = client](const auto& second) {
            return first->tcpConnection() == second->tcpConnection();
        });

        LOGI << "Client left room: " << impl().id.toString() << " " << impl().name;
        impl().clients.erase(first, last);
        LOGI << "Room size: " << impl().clients.size();
    });

    LOGD << "New user added to room: " << impl().id.toString() << " with address: " << address << ":" << port;
    emit clientAdded(newClient, address, port);
}

bool Room::clientExists(const std::shared_ptr<Client>& client) const noexcept
{
    return std::ranges::find_if(impl().clients, [&first = client](const auto& second) {
        return first->tcpConnection() ==  second->tcpConnection();
    }) != impl().clients.end();
}

} //! slk
