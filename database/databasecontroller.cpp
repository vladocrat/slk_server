#include "databasecontroller.h"

#include "database.h"

namespace slk
{

struct DatabaseController::impl_t
{
    DatabaseSettings currentSettings;
    Database db;
};

DatabaseController::DatabaseController(const DatabaseSettings& settings)
{
    createImpl();

    impl().currentSettings = settings;
}

DatabaseController::DatabaseController()
{
    createImpl();
}

DatabaseController::~DatabaseController()
{

}

bool DatabaseController::connect()
{
    return connect(impl().currentSettings);
}

bool DatabaseController::connect(const DatabaseSettings& settings)
{
    impl().currentSettings = settings;
    return impl().db.connect(settings);
}

bool DatabaseController::close()
{
    return impl().db.close();
}

} //! slk
