find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(database_tests
    userstest.h userstest.cpp
    roomstest.h roomstest.cpp)

target_link_libraries(database_tests Qt6::Core database utils configuration)
target_include_directories(database_tests PUBLIC ${CMAKE_SOURCE_DIR}/database ${CMAKE_SOURCE_DIR}/utils ${CMAKE_SOURCE_DIR}/configuration)
