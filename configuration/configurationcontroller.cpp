#include "configurationcontroller.h"

#include <QSettings>
#include <QFile>

#include "databasesettings.h"

namespace
{
static constexpr const char* DB_HOSTS_FILE = "dbhost.ini";
}

namespace slk
{

ConfigurationController::ConfigurationController() = default;

DatabaseSettings ConfigurationController::getDBSettings()
{
    QSettings settings(QString(DB_HOSTS_FILE), QSettings::Format::IniFormat);
    DatabaseSettings dbSettings;

    settings.beginGroup("Database");
    dbSettings.host = settings.value("host", "").toString().toStdString();
    dbSettings.hostAddr = settings.value("hostAddr", "").toString().toStdString();
    dbSettings.port = settings.value("port", 0).toUInt();
    dbSettings.dbName = settings.value("dbName", "").toString().toStdString();
    dbSettings.user = settings.value("user", "").toString().toStdString();
    dbSettings.requireSsl = settings.value("requireSsl", false).toBool();
    dbSettings.password = settings.value("password", "").toString().toStdString();

    settings.endGroup();

    return dbSettings;
}

std::optional<ConfigurationController::SslCertKey> ConfigurationController::getSslSettings()
{
    const auto certPath = "cert.pem";
    const auto keyPath = "key.pem";

    const auto readFile = [](const QString& filename) -> QByteArray {
        QFile file(filename);

        if (!file.open(QIODevice::ReadOnly)) {
            return {};
        }

        const auto binData = file.readAll();
        file.close();

        return binData;
    };

    const auto cert = readFile(certPath);
    const auto key = readFile(keyPath);

    if (cert.isEmpty() || key.isEmpty()) {
        return std::nullopt;
    }

    return std::make_optional(std::make_pair(cert, key));
}

} //! slk
