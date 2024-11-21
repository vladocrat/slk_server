#include "database.h"

#include <pqxx/pqxx>

#include <format>
#include <memory>
#include <iostream>

namespace
{

std::string getConnectionString(const slk::DatabaseSettings& settings)
{
    auto connectionString = std::format("host={} port={} dbname={} user={} password={} ",
                                              settings.host,
                                              settings.port,
                                              settings.dbName,
                                              settings.user,
                                              settings.password);

    connectionString += settings.requireSsl ? "sslmode=require" : "sslmode=disable";

    return connectionString;
}

} //! Utils namespace

namespace slk
{

struct Database::impl_t
{
    std::unique_ptr<pqxx::connection> connection;
};

Database::Database()
{
    createImpl();
}

Database::~Database()
{
    if (impl().connection->is_open()) {
        impl().connection->close();
    }
}

bool Database::connect(const DatabaseSettings& settings)
{
    try {
        const auto connectionString = getConnectionString(settings);
        impl().connection = std::make_unique<pqxx::connection>(connectionString);
    } catch (const std::exception &e) {
        std::cerr << e.what();
    }

    return impl().connection->is_open();
}

bool Database::close()
{
    if (impl().connection->is_open()) {
        impl().connection->close();
    }

    return impl().connection->is_open();
}

} //! slk
