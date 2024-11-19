#pragma once

#include <QObject>
#include <QHostAddress>

#include "utils/utils.h"

class QTcpSocket;
class QUuid;

namespace slk {

class Client;

class Room : public QObject
{
    Q_OBJECT
public:
    Room();
    ~Room();
    
    QUuid id() const noexcept;
    QString name() const noexcept;
    void setName(const QString& name) noexcept;
    
    void addNewClient(QTcpSocket* newClient) noexcept;
    
signals:
    void clientAlreadyExists(QTcpSocket*);
    void clientAdded(QTcpSocket*, const QHostAddress& address, const uint32_t port);
    
private:
    bool clientExists(const std::shared_ptr<Client>& client) const noexcept;
    
private:
    DECLARE_PIMPL_EX(Room)
};

} //! slk
