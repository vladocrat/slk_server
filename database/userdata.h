#pragma once

#include <string>

namespace slk
{

struct UserData
{
    std::string username;
    std::string password_hash;
    std::string mail;
};

} //! slk
