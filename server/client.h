#pragma once

#include <QHostAddress>

#include "utils.h"

class QTcpSocket;

namespace slk {

class Client : QObject
{
    Q_OBJECT
public:
    using UdpSettings = std::pair<QHostAddress, uint32_t>;

    Client(QTcpSocket* pendingConnection);
    Client(Client&& client);
    ~Client();
    
    const QTcpSocket* tcpConnection() const noexcept;
    UdpSettings udpSettings() const noexcept;

private:
    DECLARE_PIMPL_EX(Client);
};

} //! slk
