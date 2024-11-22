find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(utils STATIC
    utils.h
    pimpl.h
    print.h
    hash.h hash.cpp)

set_target_properties(utils PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(utils PRIVATE Qt6::Core)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
