#pragma once

namespace slk
{

namespace Messages
{

enum class MessageType
{
    PING, //! Simple ping command
    
    CREATE_ROOM, //! Request to create new room (name: QString)
    ROOM_CREATED, //! Response, room created successfully (room_id)
    FAILED_TO_CREATE_ROOM, //! Response, failed to create room (name, reason)
    
    CONNECT_TO_ROOM, //! Request to connect to room (Room_id)
    FAILED_TO_CONNECT_TO_ROOM //! Response, Incorrect room id (Room_id)
};

} //! Messages

} //! slk
