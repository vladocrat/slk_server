#include "client.h"

#include <QUdpSocket>
#include <QTcpSocket>
#include <QNetworkDatagram>

namespace slk {

struct Client::impl_t
{
    QTcpSocket* tcpConnection { nullptr };
    QUdpSocket* udpConnection { nullptr };
};

Client::Client(QTcpSocket* pendingConnection)
{
    if (!pendingConnection) return;
    
    createImpl();
    
    impl().tcpConnection = pendingConnection;
    impl().udpConnection = new QUdpSocket;
    
    QObject::connect(impl().udpConnection, &QUdpSocket::readyRead, impl().udpConnection, [this]() {
        while (impl().udpConnection->hasPendingDatagrams())
        {
            const auto datagram = impl().udpConnection->receiveDatagram();
        }
    });
}

Client::~Client()
{
    if (impl().tcpConnection)
    {
        impl().tcpConnection->close();
        impl().tcpConnection->deleteLater();
    }
    
    if (impl().udpConnection)
    {
        impl().udpConnection->close();
        impl().udpConnection->deleteLater();
    }
}

const QTcpSocket* Client::tcpConnection() const noexcept
{
    return impl().tcpConnection;
}

} //! slk
