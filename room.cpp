#include "room.h"

#include <QString>
#include <QUuid>
#include <QDebug>

#include <vector>

#include "client.h"

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
    qDebug() << "new room: " << impl().id;
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

void Room::addNewClient(QTcpSocket* newClient) noexcept
{
    if (!newClient) return;
    
   auto client = std::make_shared<Client>(newClient);
    
    if (clientExists(client))
    {
        emit clientAlreadyExists(newClient);
        return;
    }

    impl().clients.push_back(std::move(client));
    emit clientAdded(newClient);
}

bool Room::clientExists(const std::shared_ptr<Client>& client) const noexcept
{
    return std::find_if(impl().clients.begin(), impl().clients.end(), [first = std::move(client)](const auto& second) {
        return first->tcpConnection() ==  second->tcpConnection();
    }) != impl().clients.end();
}

} //! slk
