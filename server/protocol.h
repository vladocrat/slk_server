#pragma once

#include <cstdint>

namespace slk
{

namespace Messages
{

enum class MessageType : uint8_t
{
    PING, //! Simple ping command
    
    CREATE_ROOM, //! Request to create new room (name: QString)
    ROOM_CREATED, //! Response, room created successfully (room_id)
    FAILED_TO_CREATE_ROOM, //! Response, failed to create room (name, reason)
    
    CONNECT_TO_ROOM, //! Request to connect to room (Room_id)
    FAILED_TO_CONNECT_TO_ROOM, //! Response, Incorrect room id (Room_id)
    CONNECTED_TO_ROOM, //! Response, Connected to room successfully ()

    SEND_VOICE_MSG, //! Request and Response, voice data (slk::Device::Data)
};

} //! Messages

} //! slk
