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

target_link_libraries(${TARGET} Qt5::Widgets)
