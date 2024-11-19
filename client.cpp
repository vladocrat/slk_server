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

    QObject::connect(impl().udpConnection, &QUdpSocket::readyRead, this, [this]() {
        while (impl().udpConnection->hasPendingDatagrams())
        {
            const auto datagram = impl().udpConnection->receiveDatagram();
            QNetworkDatagram response;
            response.setData(datagram.data());
            response.setDestination(datagram.senderAddress(), datagram.senderPort());
            impl().udpConnection->writeDatagram(response);
        }
    });

    if (!impl().udpConnection->bind(QHostAddress::LocalHost, 61585))
    {
        qDebug() << "failed to bind";
        return;
    }

    qDebug() << "Binded!";
}

Client::Client(Client&& client)
{
    createImpl();
    std::exchange(impl(), client.impl());
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

Client::UdpSettings Client::udpSettings() const noexcept
{
    return std::make_pair(impl().udpConnection->localAddress(),impl().udpConnection->localPort());
}

} //! slk
