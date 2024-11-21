#include "database.h"

#include <memory>

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
        std::cerr << e.what() << std::endl;
        return false;
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

void Database::prepare(const std::string& name, const std::string& statement)
{
    assert(impl().connection);

    impl().connection->prepare(name, statement);
}

const std::unique_ptr<pqxx::connection>& Database::connection() const
{
    return impl().connection;
}

} //! slk
