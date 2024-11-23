#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "typealiases.h"

namespace slk
{

class UserData;

struct RoomData
{
    uint32_t id;
    std::string name;
    std::string guid;
    uint32_t creator_id;
    std::vector<UserData> users;

    static RoomData fromTuple(const RoomDTO&);
};

} //! slk
