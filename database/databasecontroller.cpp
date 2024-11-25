#include "databasecontroller.h"

#include <algorithm>
#include <unordered_map>

#include "database.h"
#include "validators.h"

#include "userdata.h"
#include "roomdata.h"
#include "typealiases.h"

namespace
{

static const std::unordered_map<std::string, std::string> userStatemets = {
    {"ADD_USER", "INSERT INTO public.\"Users\" (username, password_hash, mail) VALUES ($1, $2, $3) RETURNING id;"},
    {"GET_USER_BY_USERNAME", R"(SELECT u.id, u.username, u.mail, u.password_hash FROM public."Users" u WHERE username = $1;)"},
    {"GET_USER_BY_EMAIL", R"(SELECT u.id, u.username, u.mail, u.password_hash FROM public."Users" u WHERE mail = $1;)"},
    {"GET_USER_ID", "SELECT id FROM public.\"Users\" WHERE username = $1;"},
    {"GET_ALL_ROOMS_USER", R"(SELECT r.* FROM public."Rooms" r
                            JOIN (
                            SELECT room_id FROM public."User_Room" WHERE user_id = $1
                            ) subquery on r.id = subquery.room_id;)"}
};

static const std::unordered_map<std::string, std::string> roomStatements = {
    {"ADD_ROOM", R"(WITH inserted_room AS (
                         INSERT INTO public."Rooms" (name, guid)
                         VALUES ($1, $2)
                         RETURNING id)
                     INSERT INTO public."User_Room" (room_id, user_id)
                     SELECT id, $3 FROM inserted_room
                             RETURNING room_id, user_id;)"},
    {"GET_ROOM_BY_NAME", R"(SELECT * FROM public."Rooms" WHERE name = $1;)"},
    {"GET_ROOM_BY_ID", R"(SELECT * FROM public."Rooms" WHERE id = $1;)"},
    {"GET_USERS_FROM_ROOM_BY_NAME", R"(SELECT u.id, u.username, u.mail, u.password_hash
                                FROM public."Users" u
                                JOIN (
                                    SELECT ur.user_id
                                    FROM public."User_Room" ur
                                    WHERE ur.room_id = (
                                        SELECT r.id
                                        FROM public."Rooms" r
                                        WHERE r.name = $1
                                    )
                                ) subquery ON u.id = subquery.user_id;)"},
    {"GET_USERS_FROM_ROOM_BY_ID", R"(SELECT u.id, u.username, u.mail, u.password_hash
                                FROM public."Users" u
                                JOIN (
                                    SELECT ur.user_id
                                    FROM public."User_Room" ur
                                    WHERE ur.room_id = $1
                                ) subquery ON u.id = subquery.user_id;)"}
};

static const constexpr uint8_t MINIMUM_CHARACTER_REQUIRMENT = 3;

} //! Utils namespace

namespace slk
{

struct DatabaseController::impl_t
{
    DatabaseSettings currentSettings;
    Database db;
};

DatabaseController::DatabaseController(const DatabaseSettings& settings)
{
    createImpl();

    impl().currentSettings = settings;
}

DatabaseController::DatabaseController()
{
    createImpl();
}

DatabaseController::~DatabaseController()
{

}

bool DatabaseController::logIn(const std::string& mail, const std::string& password)
{
    if (mail.empty() || password.empty()) {
        return false;
    }

    EmailValidatior validator;

    if (!validator(mail)) {
        return false;
    }

    const auto res = getUserByEmail(mail);

    if (!res) {
        return false;
    }

    return res.value().password_hash == password;
}

std::optional<uint32_t> DatabaseController::registerUser(const UserData& userData)
{
    EmailValidatior validator;
    if (!validator(userData.mail)) {
        return std::nullopt;
    }

    if (userData.username.empty() || userData.mail.empty() || userData.password_hash.empty()) {
        return std::nullopt;
    }

    if (userData.username.size() < MINIMUM_CHARACTER_REQUIRMENT) {
        return std::nullopt;
    }

    if (userExists(userData)) {
        return std::nullopt;
    }

    std::vector<std::tuple<uint32_t>> id;
    const auto res = impl().db.executePrepared("ADD_USER", id, userData.username, userData.password_hash, userData.mail);

    if (!res) {
        return std::nullopt;
    }

    const auto& row = id[0];

    return std::make_optional(std::get<0>(row));
}

bool DatabaseController::userExists(const UserData& userData)
{
    if (getUserByUsername(userData.username).has_value()) {
        return true;
    }

    if (getUserByEmail(userData.mail).has_value()) {
        return true;
    }

    return  false;
}

std::optional<uint32_t> DatabaseController::getUserIdByUsername(const std::string& username)
{
    if (username.empty()) {
        return std::nullopt;
    }

    std::vector<std::tuple<uint32_t>> ret;

    const auto ok = impl().db.executePrepared("GET_USER_ID", ret, username);

    if (!ok) {
        return std::nullopt;
    }

    if (ret.empty()) {
        return std::nullopt;
    }

    const auto id = std::get<0>(ret[0]);

    return std::make_optional(id);
}

std::optional<UserData> DatabaseController::getUserByUsername(const std::string& username)
{
    std::vector<UserDTO> userDataCont;
    impl().db.executePrepared("GET_USER_BY_USERNAME", userDataCont, username);

    if (userDataCont.size() > 1 || userDataCont.empty()) {
        return std::nullopt;
    }

    const auto& userData = userDataCont[0];

    return std::make_optional(UserData::fromTuple(userData));
}

std::optional<UserData> DatabaseController::getUserByEmail(const std::string& email)
{
    std::vector<UserDTO> userDataCont;
    impl().db.executePrepared("GET_USER_BY_EMAIL", userDataCont, email);

    if (userDataCont.size() > 1 || userDataCont.empty()) {
        return std::nullopt;
    }

    const auto& userData = userDataCont[0];

    return std::make_optional(UserData::fromTuple(userData));
}

std::optional<std::vector<RoomData>> DatabaseController::getAllRoomsForUser(const uint32_t userId)
{
    std::vector<RoomDTO> rooms;
    const auto res = impl().db.executePrepared("GET_ALL_ROOMS_USER", rooms, userId);

    if (!res) {
        return std::nullopt;
    }

    std::vector<RoomData> ret;
    ret.reserve(rooms.size());
    std::ranges::for_each(rooms, [&ret](const RoomDTO& dto) {
        ret.push_back(RoomData::fromTuple(dto));
    });

    return ret;
}

std::optional<DatabaseController::RoomUserId> DatabaseController::createRoom(const uint32_t creatorId, const std::string& name, const std::string& GUID)
{
    std::vector<std::tuple<uint32_t, uint32_t>> ret;
    const auto res = impl().db.executePrepared("ADD_ROOM", ret, name, GUID, creatorId);

    if (!res) {
        return std::nullopt;
    }

    if (ret.empty()) {
        return std::nullopt;
    }

    const auto row = ret[0];

    return std::make_optional(std::make_pair(std::get<0>(row), std::get<1>(row)));
}

std::optional<RoomData> DatabaseController::getRoom(const std::string& name)
{
    std::vector<RoomDTO> ret;
    const auto res = impl().db.executePrepared("GET_ROOM_BY_NAME", ret, name);

    if (!res) {
        return std::nullopt;
    }

    if (ret.empty()) {
        //! No error, such room doesn't exist
        return std::make_optional<RoomData>();
    }

    if (ret.size() > 1) {
        return std::nullopt;
    }

    const auto row = ret[0];

    auto roomData = RoomData::fromTuple(row);

    const auto users = getRoomUsers(name);

    if (!users) {
        return std::nullopt;
    }

    roomData.users = std::move(users.value());

    return roomData;
}

std::optional<RoomData> DatabaseController::getRoom(const uint32_t id)
{
    std::vector<RoomDTO> ret;
    const auto res = impl().db.executePrepared("GET_ROOM_BY_ID", ret, id);

    if (!res) {
        return std::nullopt;
    }

    if (ret.empty()) {
        //! No error, such room doesn't exist
        return std::make_optional<RoomData>();
    }

    if (ret.size() > 1) {
        return std::nullopt;
    }

    const auto row = ret[0];

    auto roomData = RoomData::fromTuple(row);

    const auto users = getRoomUsers(id);

    if (!users) {
        return std::nullopt;
    }

    roomData.users = std::move(users.value());

    return roomData;
}

std::optional<std::vector<UserData>> DatabaseController::getRoomUsers(const std::string& name)
{
    std::vector<UserDTO> users;
    const auto res = impl().db.executePrepared("GET_USERS_FROM_ROOM_BY_NAME", users, name);

    if (!res) {
        return std::nullopt;
    }

    if (users.empty()) {
        //! It's impossible to have no users in a room
        //! Creator is added to the room on creation
        return std::nullopt;
    }

    std::vector<UserData> ret;
    ret.reserve(users.capacity());
    std::ranges::for_each(users, [&ret](const auto& tupleDTO) {
        ret.push_back(UserData::fromTuple(tupleDTO));
    });

    return std::make_optional(ret);
}

std::optional<std::vector<UserData>> DatabaseController::getRoomUsers(const uint32_t id)
{
    std::vector<UserDTO> users;
    const auto res = impl().db.executePrepared("GET_USERS_FROM_ROOM_BY_ID", users, id);

    if (!res) {
        return std::nullopt;
    }

    if (users.empty()) {
        //! It's impossible to have no users in a room
        //! Creator is added to the room on creation
        return std::nullopt;
    }

    std::vector<UserData> ret;
    ret.reserve(users.capacity());
    std::ranges::for_each(users, [&ret](const auto& tupleDTO) {
        ret.push_back(UserData::fromTuple(tupleDTO));
    });

    return std::make_optional(ret);
}

bool DatabaseController::roomExists(const std::string& roomName)
{
    return getRoom(roomName).has_value();
}

bool DatabaseController::roomExists(const uint32_t id)
{
    return getRoom(id).has_value();
}

bool DatabaseController::connect()
{
    return connect(impl().currentSettings);
}

bool DatabaseController::connect(const DatabaseSettings& settings)
{
    impl().currentSettings = settings;

    const auto ok = impl().db.connect(settings);

    if (ok) {
        prepareAllStatements();
    }

    return ok;
}

bool DatabaseController::close()
{
    return impl().db.close();
}

void DatabaseController::prepareAllStatements()
{
    const auto prepare = [this](const auto& pair) {
        impl().db.prepare(pair.first, pair.second);
    };

    std::ranges::for_each(userStatemets, prepare);
    std::ranges::for_each(roomStatements, prepare);
}

} //! slk
