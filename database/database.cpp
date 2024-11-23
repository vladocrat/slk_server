#include "database.h"

#include <memory>
#include <iostream>

#include "typealiases.h"

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

template<class T>
void setValue(T& value, const pqxx::field& fieldValue)
{
    value = fieldValue.as<T>();
}

template<class... ReturnFields,  std::size_t... Is>
void getRow(std::tuple<ReturnFields...>& row, const pqxx::row& resRow, const std::index_sequence<Is...>)
{
    return (setValue(std::get<Is>(row), resRow[Is]), ...);
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

    try {
        impl().connection->prepare(name, statement);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return;
    }
}

bool Database::executePrepared(const std::string& query)
{
    try {
        pqxx::work worker(*impl().connection);

        const auto result = worker.exec_prepared(query);

        worker.commit();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}


template<class... ReturnFields, class... Params>
bool Database::executePrepared(const std::string& query, std::vector<std::tuple<ReturnFields...>>& returnRows, const Params&... params)
{
    try {
        pqxx::work worker(*impl().connection);

        const auto result = worker.exec_prepared(query, params...);

        worker.commit();

        std::ranges::for_each(result, [&returnRows](const pqxx::row& resRow) {
            std::tuple<ReturnFields...> row;
            getRow(row, resRow, std::make_index_sequence<sizeof...(ReturnFields)>{});
            returnRows.push_back(row);
        });
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

template<class... Params>
bool Database::executePrepared(const std::string& query, const Params&... params)
{
    try {
        pqxx::work worker(*impl().connection);

        const auto result = worker.exec_prepared(query, params...);

        worker.commit();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

template bool Database::executePrepared(const std::string&, const std::string&, const std::string&, const std::string&);
template bool Database::executePrepared(const std::string&, std::vector<std::tuple<std::string, std::string, std::string>>&, const std::string&);
template bool Database::executePrepared(const std::string&, std::vector<std::tuple<uint32_t>>&, const std::string&);
template bool Database::executePrepared(const std::string&, std::vector<std::tuple<uint32_t, uint32_t>>&, const std::string&, const std::string&, const uint32_t&);
template bool Database::executePrepared(const std::string&, std::vector<UserDTO>&, const uint32_t&);
template bool Database::executePrepared(const std::string&, std::vector<RoomDTO>&, const uint32_t&);

} //! slk
