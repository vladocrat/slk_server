#pragma once

#include <QTcpServer>
#include <QSslServer>

#include "pimpl.h"

namespace slk {

namespace Messages {
enum class MessageType : uint8_t;
} //! Messages

class Server : public QSslServer
{
    Q_OBJECT
public:
    Server();
    ~Server();
    
private:
    void readData(QTcpSocket* const client, Messages::MessageType, const std::shared_ptr<QDataStream>&);

private:
    DECLARE_PIMPL_EX(Server)
};

} //! slk
