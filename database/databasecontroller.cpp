#include "databasecontroller.h"

#include <algorithm>
#include <unordered_map>

#include "database.h"
#include "userdata.h"
#include "validators.h"

namespace
{

static std::unordered_map<std::string, std::string> statemets = {
    {"ADD_USER", "INSERT INTO public.\"Users\" (username, password_hash, mail) VALUES ($1, $2, $3);"},
    {"GET_USER_BY_USERNAME", "SELECT * FROM public.\"Users\" WHERE username = $1;"},
    {"GET_USER_BY_EMAIL", "SELECT * FROM public.\"Users\" WHERE mail = $1;"}
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
    std::ranges::for_each(statemets, [this](const auto& pair) {
        impl().db.prepare(pair.first, pair.second);
    });
}

} //! slk
