#pragma once

#include <tuple>
#include <string>

namespace slk
{

class UserData;

using RoomDTO = std::tuple<uint32_t, std::string, std::string, uint32_t>;
using UserDTO = std::tuple<uint32_t, std::string, std::string, std::string>;

} //! slk
