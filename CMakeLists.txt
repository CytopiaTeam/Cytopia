cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# create the cmake project
project(
  # this will be the ${PROJECT_NAME}
  Cytopia
  # this will provide ${PROJECT_VERSION}
  VERSION 0.1
  # we are using C++
  LANGUAGES CXX
)

# set install dir here for convenience
set (CMAKE_INSTALL_PREFIX "../install")
# make sure, every binary lands in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# add builddtype as cached variable
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE}) 

if ( UNIX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# add the path to our custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

# Package Generator specific settings
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_SECTION games)
set(CPACK_PACKAGE_NAME ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_VERSION} Tech Preview")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jimmy Snails")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/JimmySnails/Cytopia")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/games/${PROJECT_NAME}")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_STRIP_FILES ON)

INCLUDE(CPack)

set (SOURCES 
src/engine/basics/log.hxx
src/engine/basics/mapEdit.hxx
src/engine/basics/mapEdit.cxx
src/engine/basics/point.hxx
src/engine/basics/resources.cxx
src/engine/basics/resources.hxx
src/engine/basics/settings.cxx
src/engine/basics/settings.hxx
src/engine/basics/signal.hxx
src/engine/basics/timer.cxx
src/engine/basics/timer.hxx
src/engine/basics/tileData.hxx
src/engine/basics/utils.hxx
src/engine/GameObjects/mapNode.cxx
src/engine/GameObjects/mapNode.hxx
src/engine/ui/bar.cxx
src/engine/ui/bar.hxx
src/engine/ui/button.cxx
src/engine/ui/button.hxx
src/engine/ui/combobox.cxx
src/engine/ui/combobox.hxx
src/engine/ui/checkbox.cxx
src/engine/ui/checkbox.hxx
src/engine/ui/frame.cxx
src/engine/ui/frame.hxx
src/engine/ui/text.cxx
src/engine/ui/text.hxx
src/engine/ui/textField.cxx
src/engine/ui/textField.hxx
src/engine/ui/tooltip.cxx
src/engine/ui/tooltip.hxx
src/engine/ui/uiElement.cxx
src/engine/ui/uiElement.hxx
src/engine/audio/soundEffect.cxx
src/engine/audio/soundEffect.cxx
src/engine/audio/music.cxx
src/engine/audio/music.hxx
src/engine/audioMixer.cxx
src/engine/audioMixer.hxx
src/engine/engine.cxx
src/engine/engine.hxx
src/engine/eventManager.cxx
src/engine/eventManager.hxx
src/engine/map.cxx
src/engine/map.hxx
src/engine/sprite.cxx
src/engine/sprite.hxx
src/engine/textureManager.cxx
src/engine/textureManager.hxx
src/engine/tileManager.cxx
src/engine/tileManager.hxx
src/engine/uiManager.cxx
src/engine/uiManager.hxx
src/engine/windowManager.cxx
src/engine/windowManager.hxx
src/ThirdParty/json.hxx
src/game.cxx
src/game.hxx
src/main.cxx
)
  
if ( MSVC )
  set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/msvc/icon.rc)
endif()

# create a new executable with the same name as the project
add_executable(${PROJECT_NAME} ${SOURCES})

# find all required dependences - if these fail the build will not continue
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

if( MINGW )
  target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
      # need to link with mingw32 before SDL2
      mingw32
  )
endif()

# link the libraries that we've found
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
    SDL::SDL2main
    SDL::SDL2_image
    SDL::SDL2_ttf
    SDL::SDL2_mixer
)

target_compile_features(
  ${PROJECT_NAME}
  PRIVATE
    # enable C++14
    cxx_std_14
)

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
    # pass the version string via the compiler
    VERSION="${PROJECT_NAME} ${PROJECT_VERSION} alpha build"
)

# copy the resources to the compiled directory
add_custom_command(
  TARGET ${PROJECT_NAME} PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)


# QT5 is used for the TileData Editing tool
find_package(Qt5 CONFIG COMPONENTS Core Widgets)

if( Qt5Core_FOUND AND Qt5Widgets_FOUND)
    message(STATUS "Found QT: ${Qt5_VERSION}")
    add_subdirectory(src/TileDataUi)
else()
    message(WARNING "No QT5 found. TileEditor Tool cannot be built.") 
    
    if ( MSVC )
      message("Install QT5 and set the Qt5_DIR environment variable to the approprate directory")
      message("eg. set Qt5_DIR=D:\\Qt\\5.12.0\\msvc2017_64")
    else()
      message("Install Qt5 dev packages via your package manager")
    endif()
endif()

# use Doxygen to generate the documentation
find_package(Doxygen)

if( DOXYGEN_FOUND )
  set (DoxyFile_src ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  set (DoxyFile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${DoxyFile_src} ${DoxyFile} @ONLY)

  message("Use target doc to build the documentation")
  add_custom_target(
    doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DoxyFile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
  )
endif()

if( MSVC )
  # set startup project for Visual Studio Builds
  set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY
      VS_STARTUP_PROJECT ${PROJECT_NAME}
  )

  # set Visual Studio working directory to the Configuration directory
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
      VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)"
  )
  
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/Resource.rc)


    # ENABLE_LANGUAGE(RC)
    # set(SOURCES Resource.rc ${SOURCES})
  

endif()




install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY resources DESTINATION .)
install(FILES ReleaseNotes.txt credits.txt DESTINATION .)
