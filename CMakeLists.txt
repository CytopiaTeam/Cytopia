cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# create the cmake project
project(
  # this will be the ${PROJECT_NAME}
  IsometricEngine
  # this will provide ${PROJECT_VERSION}
  VERSION 0.0.2
  # we are using C++
  LANGUAGES CXX
)

# add the path to our custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

# Package Generator specific settings
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_DEBIAN_PACKAGE_VERSION ${VERSION})
set(CPACK_DEBIAN_PACKAGE_SECTION games)
set(CPACK_PACKAGE_NAME "IsometricEngine")
set(CPACK_PACKAGE_DESCRIPTION "Isometric Engine Tech Preview")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jimmy Snails")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/games/IsometricEngine")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_STRIP_FILES ON)

INCLUDE(CPack)

# create a new executable with the same name as the project
add_executable(${PROJECT_NAME}
  src/engine/basics/log.hxx
  src/engine/basics/point.hxx
  src/engine/basics/resources.cxx
  src/engine/basics/resources.hxx
  src/engine/basics/settings.cxx
  src/engine/basics/settings.hxx
  src/engine/basics/timer.cxx
  src/engine/basics/timer.hxx
  src/engine/basics/vectorMatrix.cxx
  src/engine/basics/vectorMatrix.hxx
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
  src/engine/cell.cxx
  src/engine/cell.hxx
  src/engine/engine.cxx
  src/engine/engine.hxx
  src/engine/eventManager.cxx
  src/engine/eventManager.hxx
  src/engine/sprite.cxx
  src/engine/sprite.hxx
  src/engine/textureManager.cxx
  src/engine/textureManager.hxx
  src/engine/uiManager.cxx
  src/engine/uiManager.hxx
  src/engine/windowManager.cxx
  src/engine/windowManager.hxx
  src/ThirdParty/json.hxx
  src/game.cxx
  src/game.hxx
  src/main.cxx
)

# find all required dependences - if these fail the build will not continue
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

if(MINGW)
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
    # enable C++11
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

# use Doxygen to generate the documentation
find_package(Doxygen)

if(DOXYGEN_FOUND)
  set (DoxyFile_src ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  set (DoxyFile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${DoxyFile_src} ${DoxyFile} @ONLY)

  message(STATUS "${PROJECT_NAME}: You can build the documentation with the doc target.")
  add_custom_target(
    doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DoxyFile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
  )
endif()

if(MSVC)
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
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY resources DESTINATION .)