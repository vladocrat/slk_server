#include "room.h"

#include <QString>

#include <vector>

#include "client.h"

namespace slk {

struct Room::impl_t
{
    uint64_t id;
    QString name;
    std::vector<Client> clients;
};

Room::Room()
{
    createImpl();
}

Room::~Room()
{
    
}

uint64_t Room::id() const noexcept
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
    
    Client client(newClient);
    
    if (clientExists(client))
    {
        emit clientAlreadyExists(newClient);
        return;
    }

    impl().clients.push_back(client);
    emit clientAdded(newClient);
}

bool Room::clientExists(const Client& client) const noexcept
{
    return std::find(impl().clients.begin(), impl().clients.end(), client) != impl().clients.end();
}

} //! slk
