#include "hash.h"

#include <QCryptographicHash>

namespace slk
{

std::string createHash(const QString& password)
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256)
    .toHex().toStdString();
}

} //! slk
