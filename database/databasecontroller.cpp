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
    {"GET_USER_BY_USERNAME", "SELECT * FROM public.\"Users\" WHERE username = $1;"},
    {"GET_USER_BY_EMAIL", "SELECT * FROM public.\"Users\" WHERE mail = $1;"},
    {"GET_USER_ID", "SELECT id FROM public.\"Users\" WHERE username = $1;"}
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
    {"GET_USERS_FROM_ROOM_BY_NAME", R"(SELECT u.*
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
    {"GET_USERS_FROM_ROOM_BY_ID", R"(SELECT u.*
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

bool DatabaseController::logIn(const UserData& userData)
{
    UserData DBUserData;

    if (userData.mail.empty()) {
        const auto res = getUserByUsername(userData.username);

        if (!res) {
            return false;
        }

        DBUserData = res.value();
    } else {
        EmailValidatior validator;
        if (!validator(userData.mail)) {
            return false;
        }

        const auto res = getUserByEmail(userData.mail);

        if (!res) {
            return false;
        }

        DBUserData = res.value();
    }

    if (userData.password_hash != getUserByUsername(userData.username)->password_hash) {
        return false;
    }

    return true;
}

bool DatabaseController::registerUser(const UserData& userData)
{
    EmailValidatior validator;
    if (!validator(userData.mail)) {
        return false;
    }

    if (userData.username.empty() || userData.mail.empty() || userData.password_hash.empty()) {
        return false;
    }

    if (userData.username.size() < MINIMUM_CHARACTER_REQUIRMENT) {
        return false;
    }

    if (userExists(userData)) {
        return false;
    }

    return impl().db.executePrepared("ADD_USER", userData.username, userData.password_hash, userData.mail);
}

bool DatabaseController::userExists(const UserData& userData)
{
    return getUserByUsername(userData.username).has_value() || getUserByEmail(userData.mail).has_value();
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
    std::vector<std::tuple<std::string, std::string, std::string>> userDataCont;
    impl().db.executePrepared("GET_USER_BY_USERNAME", userDataCont, username);

    if (userDataCont.size() > 1 || userDataCont.empty()) {
        return std::nullopt;
    }

    const auto& userData = userDataCont[0];

    UserData user;
    user.username = std::get<0>(userData);
    user.password_hash = std::get<1>(userData);
    user.mail = std::get<2>(userData);

    return std::make_optional(user);
}

std::optional<UserData> DatabaseController::getUserByEmail(const std::string& email)
{
    std::vector<std::tuple<std::string, std::string, std::string>> userDataCont;
    impl().db.executePrepared("GET_USER_BY_EMAIL", userDataCont, email);

    if (userDataCont.size() > 1 || userDataCont.empty()) {
        return std::nullopt;
    }

    const auto& userData = userDataCont[0];

    UserData user;
    user.username = std::get<0>(userData);
    user.password_hash = std::get<1>(userData);
    user.mail = std::get<2>(userData);

    return std::make_optional(user);
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

std::optional<RoomData> DatabaseController::getRoomByName(const std::string& name)
{
    return {};
}

std::optional<RoomData> DatabaseController::getRoomById(const uint32_t id)
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
    return {};
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
    return false;
}

bool DatabaseController::roomExists(const uint32_t id)
{
    return false;
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
