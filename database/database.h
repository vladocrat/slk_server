#pragma once

#include <iostream>

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
    bool execute(const std::string& query, std::vector<std::tuple<ReturnFields...>>& returnRows, const Params&... params)
    {
        try {
            pqxx::work worker(*connection());

            const auto result = sizeof...(Params) > 0 ? worker.exec_params(query, params...)
                                                      : worker.exec(query);

            worker.commit();

            std::ranges::for_each(result, [this, &returnRows](const pqxx::row& resRow) {
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

private:
    template<class... ReturnFields,  std::size_t... Is>
    void getRow(std::tuple<ReturnFields...>& row, const pqxx::row& resRow, const std::index_sequence<Is...>)
    {
        return (setValue(std::get<Is>(row), resRow[Is]), ...);
    }

    template<class T>
    void setValue(T& value, const pqxx::field& fieldValue)
    {
        value = fieldValue.as<T>();
    }

    const std::unique_ptr<pqxx::connection>& connection() const;
private:
    DECLARE_PIMPL
};

} //! slk
