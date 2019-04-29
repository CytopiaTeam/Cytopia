set(TARGET TileDataEditor)

set(SOURCES
main.cxx
TileData.cxx
TileDataUi.cxx
)

if ( MSVC )
  set(SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/build/msvc/icon_tiledataeditor.rc)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(..)

add_executable(${TARGET} ${SOURCES})

target_compile_features(
  ${TARGET}
  PRIVATE
    # enable C++11
    cxx_std_11
)

target_link_libraries(${TARGET} CONAN_PKG::qt)

install (TARGETS ${TARGET} DESTINATION .)
