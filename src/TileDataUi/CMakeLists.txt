set(TARGET TileDataUi)

set(SOURCES
main.cxx
TileData.cxx
TileDataUi.cxx
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(..)

add_executable(${TARGET} ${SOURCES})
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 14)

find_package(Qt5 CONFIG REQUIRED COMPONENTS Core Widgets)

target_link_libraries(${TARGET} Qt5::Widgets)
