#include "databasecontroller.h"

#include <algorithm>
#include <unordered_map>

#include "database.h"

namespace
{

static std::unordered_map<std::string, std::string> statemets = {
    {"ALL_USERS", "SELECT * FROM \"mytable\""},
};

} //! Utils namespace

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

int DatabaseController::getValue()
{
    int id;
    impl().db.execute(statemets["ALL_USERS"], std::tie(id));
    return id;
}

bool DatabaseController::connect()
{
    return connect(impl().currentSettings);
}

bool DatabaseController::connect(const DatabaseSettings& settings)
{
    impl().currentSettings = settings;

    const auto ok = impl().db.connect(settings);

    if (ok) {
        prepareAllStatements();
    }

    return ok;
}

bool DatabaseController::close()
{
    return impl().db.close();
}

void DatabaseController::prepareAllStatements()
{
    std::ranges::for_each(statemets, [this](const auto& pair) {
        impl().db.prepare(pair.first, pair.second);
    });
}

} //! slk
