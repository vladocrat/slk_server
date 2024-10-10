#pragma once

#include <QObject>

#include "utils/utils.h"

class QTcpSocket;

namespace slk {

class Client;

class Room : public QObject
{
    Q_OBJECT
public:
    Room();
    ~Room();
    
    uint64_t id() const noexcept;
    QString name() const noexcept;
    
    void addNewClient(QTcpSocket* newClient) noexcept;
    
signals:
    void clientAlreadyExists(QTcpSocket*);
    void clientAdded(QTcpSocket*);
    
private:
    bool clientExists(const Client&) const noexcept;
    
private:
    DECLARE_PIMPL_EX(Room)
};

} //! slk
