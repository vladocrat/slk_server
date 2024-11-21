find_package(PostgreSQL REQUIRED)

add_library(database
    databasecontroller.h databasecontroller.cpp
    databasesettings.h databasesettings.cpp
    database.h database.cpp)

set(COMPILE_GENERATED ${libpqxx_BINARY_DIR}/include)

target_link_libraries(database PRIVATE ${PQXX_LIBRARIES} pqxx)
target_include_directories(database PRIVATE ${CMAKE_SOURCE_DIR}/utils ${COMPILE_GENERATED} ${libpqxx_SOURCE_DIR}/include ${PostgreSQL_INCLUDE_DIRS})
