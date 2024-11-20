find_package(PostgreSQL REQUIRED)

add_library(database
    databasecontroller.h databasecontroller.cpp
    databasesettings.h databasesettings.cpp)

set(COMPILE_GENERATED ${libpqxx_BINARY_DIR}/include)
message(${COMPILE_GENERATED})

target_link_libraries(database PRIVATE ${PQXX_LIBRARIES} utils)
target_include_directories(database PRIVATE ${CMAKE_SOURCE_DIR}/../utils ${COMPILE_GENERATED} ${libpqxx_SOURCE_DIR}/include ${PostgreSQL_INCLUDE_DIRS})
