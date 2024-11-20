#pragma once

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QUuid>
#include <QDebug>

#include <memory>
#include <qhostaddress.h>

#include "message.h"
#include "protocol.h"

namespace
{

template<class... Args>
std::unique_ptr<QByteArray> writeToByteArray(Args... arg)
{
    auto array = std::make_unique<QByteArray>();
    QDataStream stream(&(*array), QIODevice::WriteOnly);

    ((stream << arg), ...);
    return std::move(array);
}

std::unique_ptr<QByteArray> writeToByteArray(const QString& str)
{
    return std::make_unique<QByteArray>(str.toUtf8());
}

std::unique_ptr<QByteArray> writeToByteArray(const QUuid& id)
{
    return std::make_unique<QByteArray>(id.toRfc4122());
}

}

namespace slk
{

class MessageFactory
{
public:
    template<class... Args>
    static std::unique_ptr<QByteArray> create(Messages::MessageType type, Args... args)
    {
        Message msg;
        msg.type = type;
        msg.payload = *writeToByteArray(args...);
        return std::move(writeToByteArray(msg));
    }

private:
    MessageFactory();
};

} //! slk
