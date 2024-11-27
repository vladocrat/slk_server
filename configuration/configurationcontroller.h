#pragma once

#include <QByteArray>

namespace slk
{

struct DatabaseSettings;

class ConfigurationController
{
public:
    ConfigurationController();

    using SslCertKey = std::pair<QByteArray, QByteArray>;

    static DatabaseSettings getDBSettings();
    static std::optional<SslCertKey> getSslSettings();
};

} //! slk
