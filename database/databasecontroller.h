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
    bool logIn(const std::string& mail, const std::string& password);
    bool userExists(const UserData&);
    std::optional<uint32_t> registerUser(const UserData& userData);
    std::optional<uint32_t> getUserIdByUsername(const std::string& username);
    std::optional<UserData> getUserByUsername(const std::string& username);
    std::optional<UserData> getUserByEmail(const std::string& email);
    std::optional<std::vector<RoomData>> getAllRoomsForUser(const uint32_t userId);

    //! Rooms
    using RoomUserId = std::pair<const uint32_t, const uint32_t>;

    std::optional<RoomUserId> createRoom(const uint32_t creatorId, const std::string& name, const std::string& GUID);
    std::optional<RoomData> getRoom(const std::string& name);
    std::optional<RoomData> getRoom(const uint32_t id);
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
