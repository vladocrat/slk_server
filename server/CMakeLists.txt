set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_library(server
    server.h server.cpp
    server.h server.cpp
    room.h room.cpp
    client.h client.cpp
    protocol.h
    messageparser.h messageparser.cpp
    messagefactory.h messagefactory.cpp
    message.h message.cpp)

message(${CMAKE_SOURCE_DIR}/utils)

target_link_libraries(server PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/utils)
