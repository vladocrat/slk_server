cmake_minimum_required(VERSION 3.16)

project(VoIP_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rd_party)
add_subdirectory(utils)
add_subdirectory(database)
add_subdirectory(server)
add_subdirectory(configuration)
add_subdirectory(tests)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(NUM_CORES ${CMAKE_MAX_NUM_THREADS})
set(CMAKE_BUILD_PARALLEL_LEVEL ${NUM_CORES} CACHE STRING "Maximum number of concurrent compilation jobs")

add_executable(VoIP_server
  main.cpp
)

target_link_libraries(VoIP_server PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network server configuration)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(COPY "dbhost.ini" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY "dbhost.ini" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tests)
endif()

include(GNUInstallDirs)
install(TARGETS VoIP_server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
