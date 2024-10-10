#pragma once

#include <QByteArray>

#include "protocol.h"

namespace slk
{
    
class MessageParser
{
public:
    struct Message
    {
        Messages::MessageType type;
        QByteArray payload;
    };
    
    MessageParser();
    
    static Message parse(const QByteArray& data) noexcept;
};

} //! slk
