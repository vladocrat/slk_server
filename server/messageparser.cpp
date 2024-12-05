#include "messageparser.h"

#include <QDataStream>

namespace slk
{

MessageParser::CommandResult MessageParser::parseCommand(const QByteArray& data) noexcept
{
    QDataStream stream(data);
    uint64_t size;
    QByteArray payload;
    stream >> size;
    stream >> payload;

    auto payloadStream = std::make_shared<QDataStream>(payload);
    uint8_t type;
    *payloadStream.get() >> type;
    return std::make_pair(static_cast<Messages::MessageType>(type), payloadStream);
}

} //! slk;
