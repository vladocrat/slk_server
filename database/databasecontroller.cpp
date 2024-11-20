#include "databasecontroller.h"

#include <pqxx/pqxx>

namespace slk
{

struct DatabaseController::impl_t
{
    DatabaseSettings currentSettings;
};

DatabaseController::DatabaseController(const DatabaseSettings& settings)
{
    createImpl();

    impl().currentSettings = settings;
}

DatabaseController::DatabaseController()
{

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
    return {};
}

} //! slk
