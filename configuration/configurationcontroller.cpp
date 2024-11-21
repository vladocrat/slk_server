#include "configurationcontroller.h"

#include <QSettings>

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

} //! slk
