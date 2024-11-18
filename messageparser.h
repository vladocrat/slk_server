#pragma once

#include <QByteArray>

#include "protocol.h"

namespace slk
{
    
class MessageParser
{
public:
    using ParsingResult = std::pair<Messages::MessageType, std::unique_ptr<QDataStream>>;
    
    struct Message
    {
        Messages::MessageType type;
        QByteArray payload;
        
        friend QDataStream& operator<<(QDataStream& out, const Message& msg);
    };
    
    MessageParser();
    
    static ParsingResult parse(const QByteArray& data) noexcept;
};

} //! slk
