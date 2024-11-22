find_package(Qt6 REQUIRED COMPONENTS Core Network Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

enable_testing(true)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(database)

add_executable(tests main.cpp)

target_link_libraries(tests PUBLIC Qt6::Test Qt6::Core database_tests)
