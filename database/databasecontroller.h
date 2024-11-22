#pragma once

#include <optional>

#include "databasesettings.h"
#include "pimpl.h"

namespace slk
{

class UserData;

class DatabaseController final
{
public:
    DatabaseController(const DatabaseSettings& settings);
    DatabaseController();
    ~DatabaseController();

    bool logIn(const UserData& userData);
    bool registerUser(const UserData& userData);
    bool userExists(const UserData&);
    std::optional<UserData> getUserByUsername(const std::string& username);
    std::optional<UserData> getUserByEmail(const std::string& email);

    bool connect();
    bool connect(const DatabaseSettings& settings);
    bool close();

private:
    void prepareAllStatements();

private:
    DECLARE_PIMPL
};

} //! slk
