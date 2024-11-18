#pragma once

#include "utils/utils.h"

class QTcpSocket;

namespace slk {

class Client
{
public:
    Client(QTcpSocket* pendingConnection);
    Client(Client&& client);
    ~Client();
    
    const QTcpSocket* tcpConnection() const noexcept;
        
private:
    DECLARE_PIMPL_EX(Client);
};

} //! slk
