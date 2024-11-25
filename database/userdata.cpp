#include "userdata.h"

namespace slk
{

UserData UserData::fromTuple(const UserDTO& dto)
{
    UserData ret;
    ret.id = std::get<0>(dto);
    ret.username = std::get<1>(dto);
    ret.mail = std::get<2>(dto);
    ret.password_hash = std::get<3>(dto);

    return ret;
}

} //! slk
