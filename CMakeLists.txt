cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# create the cmake project
project(
  # this will be the ${PROJECT_NAME}
  Cytopia
  # this will provide ${PROJECT_VERSION}
  VERSION 0.1.1
  # Description
  DESCRIPTION "A city-building game"
  # we are using C++
  LANGUAGES CXX
)

# get linux distribution
if ( UNIX )
  execute_process(COMMAND head -1 /etc/os-release OUTPUT_VARIABLE linux_distribution)
endif()

# Configuration for packaging
include(build/cmake/CpackConfiguration.cmake)

# Compile options
include(build/cmake/CompileOptions.cmake)

# set install dir here for convenience
set (CMAKE_INSTALL_PREFIX "../install")

# make sure, every binary lands in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# add builddtype as cached variable
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})

message(STATUS "Building Project:  ${PROJECT_NAME}") 
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE}) 

if ( UNIX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# add the path to our custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake_modules/")

set (SOURCES 
  src/engine/basics/camera.cxx
  src/engine/basics/isoMath.cxx
  src/engine/basics/mapEdit.cxx
  src/engine/basics/settings.cxx
  src/engine/basics/timer.cxx
  src/engine/basics/utils.cxx
  src/engine/GameObjects/mapNode.cxx
  src/engine/ui/basics/uiElement.cxx
  src/engine/ui/basics/buttonGroup.cxx
  src/engine/ui/basics/Layout.cxx
  src/engine/ui/widgets/bar.cxx
  src/engine/ui/widgets/button.cxx
  src/engine/ui/widgets/combobox.cxx
  src/engine/ui/widgets/checkbox.cxx
  src/engine/ui/widgets/frame.cxx
  src/engine/ui/widgets/Image.cxx
  src/engine/ui/widgets/text.cxx
  src/engine/ui/widgets/textField.cxx
  src/engine/ui/widgets/tooltip.cxx
  src/engine/ui/menuGroups/MenuGroupBuild.cxx
  src/engine/audio/soundEffect.cxx
  src/engine/audio/soundEffect.cxx
  src/engine/audio/music.cxx
  src/engine/audioMixer.cxx
  src/engine/engine.cxx
  src/engine/eventManager.cxx
  src/engine/map.cxx
  src/engine/sprite.cxx
  src/engine/resourcesManager.cxx
  src/engine/tileManager.cxx
  src/engine/uiManager.cxx
  src/engine/windowManager.cxx
  src/game.cxx
  src/main.cxx
)
  
if ( MSVC )
  set(SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/build/msvc/icon_cytopia.rc)
endif()

if ( APPLE )
  file(GLOB_RECURSE MAC_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/*)
  file(GLOB_RECURSE MAC_EXTERN_RESOURCES ${PROJECT_SOURCE_DIR}/Cytopia_Resources/*)

  # not working now, we need a special icon file for apple
  set(ICON_PATH ${PROJECT_SOURCE_DIR}/resources/images/app_icons/cytopia_icon_iso.icns)
  set(MACOSX_BUNDLE_ICON_FILE  cytopia_icon_iso.icns)

  # set each file as resources file for the app image.
  foreach (resource_file ${MAC_RESOURCES})
    get_filename_component(buildDirRelFilePath ${resource_file} DIRECTORY)
    string(REPLACE  "${CMAKE_CURRENT_SOURCE_DIR}/" "" buildDirRelFilePath ${buildDirRelFilePath} )
    set_source_files_properties(${resource_file} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${buildDirRelFilePath})
  endforeach()

  foreach (resource_file ${MAC_EXTERN_RESOURCES})
    get_filename_component(buildDirRelFilePath ${resource_file} DIRECTORY)
    string(REPLACE  "${CMAKE_CURRENT_SOURCE_DIR}/Cytopia_Resources/" "" buildDirRelFilePath ${buildDirRelFilePath} )
    set_source_files_properties("${resource_file}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources/resources/audio/${buildDirRelFilePath})
  endforeach()

  # set_source_files_properties(${PROJECT_SOURCE_DIR}/Cytopia_Resources/music  PROPERTIES MACOSX_PACKAGE_LOCATION Resources/resources/audio/music)
  set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${ICON_PATH} ${MAC_RESOURCES} ${MAC_EXTERN_RESOURCES})

else()
  # create a new executable with the same name as the project
  add_executable(${PROJECT_NAME} ${SOURCES})
endif()

# find all required dependences - if these fail the build will not continue
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

find_package(ZLIB)

if( NOT ZLIB_FOUND )
  message (FATAL_ERROR "Could not find zlib. Set ZLIB_DIR env variable or use Cytopia_ExternLibs git submodule")
endif()

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
    ${ZLIB_LIBRARIES}
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${ZLIB_INCLUDE_DIRS}
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

# Compile options
target_compile_options(
  ${PROJECT_NAME}
  PRIVATE

  PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}

  INTERFACE
)

# copy the resources to the compiled directory
add_custom_command(
  TARGET ${PROJECT_NAME} PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

if (EXISTS ${PROJECT_SOURCE_DIR}/Cytopia_Resources)
  add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Cytopia_Resources/music $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/audio/music
  )

else ()
  message(WARNING "No Resources directory found. Please init git submodules with following command:")
  message("git submodule update --init --recursive")
endif()

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
  set (DoxyFile_src ${PROJECT_SOURCE_DIR}/Doxyfile)
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
    DIRECTORY ${PROJECT_SOURCE_DIR}
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
if ( NOT APPLE )
  install(DIRECTORY resources DESTINATION .)

  if ( EXISTS ${PROJECT_SOURCE_DIR}/Cytopia_Resources )
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Cytopia_Resources/music DESTINATION ./resources/audio)
  endif()
endif()

install(FILES ReleaseNotes.txt credits.txt DESTINATION .)


#on windows, we need to deploy the SDL2 dynamic libraries
if ( WIN32 )
  set (SDL_RUNTIME_LIBRARIES
  ${SDL2_RUNTIME_LIBRARY_DIR}/libFLAC-8.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libfreetype-6.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libjpeg-9.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libmodplug-1.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libmpg123-0.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libogg-0.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libopus-0.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libopusfile-0.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libpng16-16.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libtiff-5.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libvorbis-0.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libvorbisfile-3.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/libwebp-7.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/SDL2.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/SDL2_image.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/SDL2_mixer.dll
  ${SDL2_RUNTIME_LIBRARY_DIR}/SDL2_ttf.dll
  ${ZLIB_RUNTIME_LIBRARY}
  )

  # copy the qt libraries over to the build directory so the tool can be started
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SDL_RUNTIME_LIBRARIES}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )

  #install the dynamic libraries so we have them in the deployment
  install (FILES ${SDL_RUNTIME_LIBRARIES} DESTINATION .)
endif()

if ( APPLE )
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE RESOURCE TRUE)
endif()
