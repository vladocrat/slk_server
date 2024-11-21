find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(configuration
    configurationcontroller.h configurationcontroller.cpp)

target_link_libraries(configuration PRIVATE Qt${QT_VERSION_MAJOR}::Core database)
target_include_directories(configuration PRIVATE ${CMAKE_SOURCE_DIR}/database)
