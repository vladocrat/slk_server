#pragma once

#include <string>

#include "typealiases.h"

namespace slk
{

struct UserData
{
    uint32_t id;
    std::string username;
    std::string mail;
    std::string password_hash;

    static UserData fromTuple(const UserDTO&);
};

} //! slk
