#pragma once

#include <QObject>
#include <QHostAddress>

#include "pimpl.h"

class QSslSocket;
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
    
    void addNewClient(QSslSocket* newClient) noexcept;
    
signals:
    void clientAlreadyExists(QSslSocket*);
    void clientAdded(QSslSocket*, const QHostAddress& address, const uint32_t port);
    
private:
    bool clientExists(const std::shared_ptr<Client>& client) const noexcept;
    
private:
    DECLARE_PIMPL_EX(Room)
};

} //! slk
