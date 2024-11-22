#pragma once

#include <pqxx/pqxx>

#include "pimpl.h"
#include "databasesettings.h"

namespace slk
{

class Database
{
public:
    Database();
    ~Database();

    bool connect(const DatabaseSettings& settings);
    bool close();
    void prepare(const std::string& name, const std::string& statement);

    template<class... ReturnFields, class... Params>
    bool executePrepared(const std::string& query, std::vector<std::tuple<ReturnFields...>>& returnRows, const Params&... params);

    template<class... Params>
    bool executePrepared(const std::string& query, const Params&... params);

    bool executePrepared(const std::string& query);

private:
    DECLARE_PIMPL
};

} //! slk
