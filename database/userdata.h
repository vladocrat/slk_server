#pragma once

#include <string>

#include "typealiases.h"

namespace slk
{

struct UserData
{
    std::string username;
    std::string password_hash;
    std::string mail;

    static UserData fromTuple(const UserDTO&);
};

} //! slk
