#include "message.h"

#include <QDataStream>

namespace slk
{

QDataStream& operator<<(QDataStream& out, const Message& msg) {
    out << msg.type;
    out << msg.payload;
    return out;
};

} //! slk
