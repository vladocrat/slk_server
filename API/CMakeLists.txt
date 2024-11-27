set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_library(api
    apihandler.h apihandler.cpp)

target_link_libraries(api Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    utils
    alog
    configuration)

target_include_directories(api PUBLIC ${CMAKE_SOURCE_DIR}/configuration)
