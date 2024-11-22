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
    std::optional<uint32_t> getUserIdByUsername(const std::string& username);
    std::optional<UserData> getUserByUsername(const std::string& username);
    std::optional<UserData> getUserByEmail(const std::string& email);

    bool createRoom(const uint32_t creatorId, const std::string& name, const std::string& GUID);

    bool connect();
    bool connect(const DatabaseSettings& settings);
    bool close();

private:
    void prepareAllStatements();

private:
    DECLARE_PIMPL
};

} //! slk
