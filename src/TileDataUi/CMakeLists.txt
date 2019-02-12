set(TARGET TileDataUi)

set(SOURCES
main.cxx
TileData.cxx
TileDataUi.cxx
)

if ( MSVC )
  set(SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/msvc/icon_tiledataeditor.rc)
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

target_link_libraries(${TARGET} Qt5::Widgets)

install (TARGETS ${TARGET} DESTINATION .)

#on windows, we need to deploy the libraries
if ( WIN32 )
  install (FILES $<TARGET_FILE:Qt5::Core> DESTINATION .)
  install (FILES $<TARGET_FILE:Qt5::Gui> DESTINATION .)
  install (FILES $<TARGET_FILE:Qt5::Widgets> DESTINATION .)
  install (FILES $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> DESTINATION platforms)

  # copy the qt libraries over to the build directory so the tool can be started
  add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:Qt5::Core>
    $<TARGET_FILE:Qt5::Gui>
    $<TARGET_FILE:Qt5::Widgets>
    $<TARGET_FILE_DIR:${TARGET}>
  )
endif()
