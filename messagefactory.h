#pragma once

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QUuid>

#include <memory>

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
        switch (type)
        {
        case Messages::MessageType::PING:
        {
            Message msg;
            msg.type = Messages::MessageType::PING;
            msg.payload = {};
            return std::move(writeToByteArray(msg));
        }
        case Messages::MessageType::CREATE_ROOM:
        {
            return {};
        }
        case Messages::MessageType::ROOM_CREATED:
        {
            Message msg;
            msg.type = Messages::MessageType::ROOM_CREATED;
            msg.payload = *writeToByteArray(args...);
            return std::move(writeToByteArray(msg));
        }
        case Messages::MessageType::FAILED_TO_CREATE_ROOM:
        {
            Message msg;
            msg.type = Messages::MessageType::FAILED_TO_CREATE_ROOM;
            msg.payload = *writeToByteArray(args...);
            return std::move(writeToByteArray(msg));
        }
        case Messages::MessageType::CONNECT_TO_ROOM:
            [[fallthrough]];
        case Messages::MessageType::FAILED_TO_CONNECT_TO_ROOM:
            [[fallthrough]];
        case Messages::MessageType::CONNECTED_TO_ROOM:
        {
            Message msg;
            msg.type = Messages::MessageType::CONNECTED_TO_ROOM;
            msg.payload = *writeToByteArray(args...);
            return std::move(writeToByteArray(msg));
        }
        case Messages::MessageType::SEND_VOICE_MSG:
            break;
        }

        return {};
    }

private:
    MessageFactory();
};

} //! slk
