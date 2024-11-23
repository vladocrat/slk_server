#pragma once

#include <optional>
#include <vector>

#include "databasesettings.h"
#include "pimpl.h"

namespace slk
{

class UserData;
class RoomData;

class DatabaseController final
{
public:
    DatabaseController(const DatabaseSettings& settings);
    DatabaseController();
    ~DatabaseController();

    //! Users
    bool logIn(const UserData& userData);
    bool registerUser(const UserData& userData);
    bool userExists(const UserData&);
    std::optional<uint32_t> getUserIdByUsername(const std::string& username);
    std::optional<UserData> getUserByUsername(const std::string& username);
    std::optional<UserData> getUserByEmail(const std::string& email);

    //! Rooms
    using RoomUserId = std::pair<const uint32_t, const uint32_t>;

    std::optional<RoomUserId> createRoom(const uint32_t creatorId, const std::string& name, const std::string& GUID);
    std::optional<RoomData> getRoomByName(const std::string& name);
    std::optional<RoomData> getRoomById(const uint32_t);
    std::optional<std::vector<UserData>> getRoomUsers(const std::string& name);
    std::optional<std::vector<UserData>> getRoomUsers(const uint32_t id);
    bool roomExists(const std::string& roomName);
    bool roomExists(const uint32_t id);

    //! General
    bool connect();
    bool connect(const DatabaseSettings& settings);
    bool close();

private:
    void prepareAllStatements();

private:
    DECLARE_PIMPL
};

} //! slk
