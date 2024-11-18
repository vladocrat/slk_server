#include "messageparser.h"

#include <QDataStream>

namespace slk
{

MessageParser::MessageParser() {}

MessageParser::ParsingResult MessageParser::parse(const QByteArray& data) noexcept
{
    QDataStream stream(data);
    uint64_t size;
    QByteArray payload;
    stream >> size;
    stream >> payload;
    
    auto payloadStream = std::make_unique<QDataStream>(payload);
    uint8_t type;
    *payloadStream.get() >> type;
    return std::make_pair(static_cast<Messages::MessageType>(type), std::move(payloadStream));
}

QDataStream& operator<<(QDataStream& out, const MessageParser::Message& msg)
{
    out << msg.type;
    out << msg.payload;
    return out;
}

} //! slk;
