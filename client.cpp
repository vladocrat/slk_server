#include "client.h"

#include <QUdpSocket>
#include <QTcpSocket>

namespace slk {

struct Client::impl_t
{
    QTcpSocket* tcpConnection { nullptr };
    QUdpSocket* udpConnection { nullptr };
};

Client::Client()
{
    createImpl();
}

Client::~Client()
{
    if (impl().tcpConnection)
    {
        impl().tcpConnection->close();
        delete impl().tcpConnection;
    }
    
    if (impl().udpConnection)
    {
        impl().udpConnection->close();
        delete impl().udpConnection;
    }
}

} //! slk
