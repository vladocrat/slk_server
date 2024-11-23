#include "roomdata.h"

#include "userdata.h"

namespace slk
{

RoomData RoomData::fromTuple(const RoomDTO& dto)
{
    RoomData ret;
    ret.id = std::get<0>(dto);
    ret.name = std::get<1>(dto);
    ret.guid = std::get<2>(dto);
    ret.creator_id = std::get<3>(dto);
    ret.users = std::vector<UserData>();
    return ret;
}

}