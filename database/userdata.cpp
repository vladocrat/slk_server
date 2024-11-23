#include "userdata.h"

namespace slk
{

UserData UserData::fromTuple(const UserDTO& dto)
{
    UserData ret;
    ret.username = std::get<0>(dto);
    ret.password_hash = std::get<1>(dto);
    ret.mail = std::get<2>(dto);

    return ret;
}

} //! slk
