cmake_minimum_required(VERSION 3.16)

project(VoIP_server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)

add_subdirectory(utils)
add_subdirectory(libpqxx)

include_directories(libpqxx)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(VoIP_server
  main.cpp
  server.h server.cpp
  room.h room.cpp
  client.h client.cpp
  protocol.h
  messageparser.h messageparser.cpp
  messagefactory.h messagefactory.cpp
  message.h message.cpp
)

target_link_libraries(VoIP_server Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network libpqxx)

include(GNUInstallDirs)
install(TARGETS VoIP_server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
