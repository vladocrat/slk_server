#include "messageparser.h"

#include <QDataStream>

namespace slk
{

MessageParser::MessageParser() {}

MessageParser::ParsingResult MessageParser::parse(const QByteArray& data) noexcept
{
    QDataStream stream(data);
    uint32_t size;
    QByteArray payload;
    stream >> size;
    stream >> payload;
    
    auto payloadStream = std::make_shared<QDataStream>(payload);
    Messages::MessageType type;
    *payloadStream.get() >> type;
    return std::make_pair(type, payloadStream);
}

QDataStream& operator<<(QDataStream& out, const MessageParser::Message& msg)
{
    out << msg.type;
    out << msg.payload;
    return out;
}

} //! slk;
