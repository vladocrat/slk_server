#pragma once

#include <QHostAddress>

#include "pimpl.h"

class QSslSocket;

namespace slk {

class Client : QObject
{
    Q_OBJECT
public:
    using UdpSettings = std::pair<QHostAddress, uint32_t>;

    Client(QSslSocket* pendingConnection);
    Client(Client&& client);
    ~Client();
    
    const QSslSocket* tcpConnection() const noexcept;
    UdpSettings udpSettings() const noexcept;

private:
    DECLARE_PIMPL_EX(Client);
};

} //! slk
