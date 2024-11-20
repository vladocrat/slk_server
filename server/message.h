#pragma once

#include <QByteArray>

#include "protocol.h"

class QDataStream;

namespace slk
{

struct Message
{
    Messages::MessageType type;
    QByteArray payload;

    friend QDataStream& operator<<(QDataStream& out, const Message& msg);
};


} //! slk
