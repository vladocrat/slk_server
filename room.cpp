#include "room.h"

#include <vector>

#include "client.h"

namespace slk {

struct Room::impl_t
{
    std::vector<Client> clients;
};

Room::Room()
{
    createImpl();
}

Room::~Room()
{
    
}

} //! slk
