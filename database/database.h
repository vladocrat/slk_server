#pragma once

#include <pqxx/pqxx>

#include <iostream>

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

    template<class... Values, class... Params>
    void execute(const std::string& query, std::tuple<Values&...> values, Params&&... params)
    {
        try {
            pqxx::work worker(*connection().get());

            const auto result = worker.exec_params(query, params...);

            worker.commit();

            if (result.empty()) {
                return;
            }

            const auto& row = result[0];
            std::apply([&row](auto&... fields) {
                size_t index = 0;
                ((fields = row[index++].template as<std::decay_t<decltype(fields)>>()), ...);
            }, values);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return;
        }
    }

private:
    const std::unique_ptr<pqxx::connection>& connection() const;

private:
    DECLARE_PIMPL
};

} //! slk
