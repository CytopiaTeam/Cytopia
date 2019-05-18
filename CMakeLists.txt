cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(BUILD_TILEDATA_EDITOR OFF CACHE BOOL "Build Cytopia TileData Editor")
set(USE_PACKAGE_MANAGER ON CACHE BOOL "Use conan for managing packages")
set(USE_PMM OFF CACHE BOOL "Use PMM for managing conan")
set(ENABLE_SDL2_MIXER ON CACHE BOOL "Enable SDL2 Mixer")


# create the cmake project
project(
        # this will be the ${PROJECT_NAME}
        Cytopia
        # this will provide ${PROJECT_VERSION}
        VERSION 0.2
        # Description
        DESCRIPTION "A city-building game"
        # we are using C++
        LANGUAGES CXX
)

# on macOS, usr/local/lib is not in the default search path for libraries. Homebrew install it's libraries there though.
if (APPLE AND NOT USE_PACKAGE_MANAGER)
  LINK_DIRECTORIES(${LINK_DIRECTORIES} /usr/local/lib)
  include_directories(${INCLUDE_DIRECTORIES} /usr/local/include/)
endif()

if (USE_PACKAGE_MANAGER)

    if (USE_PMM)
        include(cmake/pmm.cmake)

        pmm(DEBUG CONAN REMOTES AFG https://api.bintray.com/conan/anotherfoxguy/conan-packages BINCRAFTERS)

    else ()
        include(cmake/conan.cmake)

        conan_check()

        conan_add_remote(NAME AFG URL https://api.bintray.com/conan/anotherfoxguy/conan-packages)
        conan_add_remote(NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan)

        SET(CONAN_DEPS
                "sdl2/2.0.9@bincrafters/stable"
                "sdl2_ttf/2.0.15@bincrafters/stable"
                "sdl2_image/2.0.4@bincrafters/stable"
                "libpng/1.6.36@bincrafters/stable"
                "freetype/2.9.1@bincrafters/stable"
                "OpenSSL/1.1.1a@conan/stable"
                "libnoise/1.0.0@anotherfoxguy/stable"
                )
        if (ENABLE_SDL2_MIXER)
            list(APPEND CONAN_DEPS "sdl2_mixer/2.0.4@anotherfoxguy/stable")
        endif()

        if (BUILD_TILEDATA_EDITOR)
            list(APPEND CONAN_DEPS "qt/5.12.3@bincrafters/stable")
        endif ()

        set(CONAN_ARGS
            BASIC_SETUP
            CMAKE_TARGETS
        )

        if(NOT DEFINED ENV{CI})
          list(APPEND CONAN_ARGS NO_OUTPUT_DIRS)
        endif()

        conan_cmake_run(REQUIRES ${CONAN_DEPS}
                OPTIONS sdl2_mixer:opus=False
                BUILD missing
                ${CONAN_ARGS}
            )

    endif ()

else (USE_PACKAGE_MANAGER)

    # add the path to our custom find modules
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_image REQUIRED SDL2_image)
    pkg_check_modules(SDL2_ttf REQUIRED SDL2_ttf)
    pkg_check_modules(ZLIB REQUIRED zlib)

    if (ENABLE_SDL2_MIXER)
        pkg_check_modules(SDL2_mixer REQUIRED SDL2_mixer)
    endif()

    find_package(LIBNOISE REQUIRED)

endif (USE_PACKAGE_MANAGER)

# get linux distribution
if (UNIX AND NOT APPLE)
    execute_process(COMMAND head -1 /etc/os-release OUTPUT_VARIABLE linux_distribution)
endif ()

# Configuration for packaging
include(cmake/CpackConfiguration.cmake)

# Compile options
include(cmake/CompileOptions.cmake)

# set install dir here for convenience
set(CMAKE_INSTALL_PREFIX "../install")

# make sure, every binary lands in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

message(STATUS "Building Project:  ${PROJECT_NAME}")
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE})

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()

set(SOURCES
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

if (ENABLE_SDL2_MIXER)
    set(SOURCES ${SOURCES}
        src/engine/audio/soundEffect.cxx
        src/engine/audio/soundEffect.cxx
        src/engine/audio/music.cxx
        src/engine/audioMixer.cxx
        )
endif()

if (MSVC)
    set(SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/build/msvc/icon_cytopia.rc)
endif ()

if (APPLE)
    file(GLOB_RECURSE MAC_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/*)
    file(GLOB_RECURSE MAC_EXTERN_RESOURCES ${PROJECT_SOURCE_DIR}/Cytopia_Resources/*)

    # not working now, we need a special icon file for apple
    set(ICON_PATH ${PROJECT_SOURCE_DIR}/resources/images/app_icons/cytopia_icon_iso.icns)
    set(MACOSX_BUNDLE_ICON_FILE cytopia_icon_iso.icns)

    # set each file as resources file for the app image.
    foreach (resource_file ${MAC_RESOURCES})
        get_filename_component(buildDirRelFilePath ${resource_file} DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" buildDirRelFilePath ${buildDirRelFilePath})
        set_source_files_properties(${resource_file} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${buildDirRelFilePath})
    endforeach ()

    foreach (resource_file ${MAC_EXTERN_RESOURCES})
        get_filename_component(buildDirRelFilePath ${resource_file} DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/Cytopia_Resources/" "" buildDirRelFilePath ${buildDirRelFilePath})
        set_source_files_properties("${resource_file}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources/resources/audio/${buildDirRelFilePath})
    endforeach ()

    # set_source_files_properties(${PROJECT_SOURCE_DIR}/Cytopia_Resources/music  PROPERTIES MACOSX_PACKAGE_LOCATION Resources/resources/audio/music)
    set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${ICON_PATH} ${MAC_RESOURCES} ${MAC_EXTERN_RESOURCES})

else ()
    # create a new executable with the same name as the project
    add_executable(${PROJECT_NAME} ${SOURCES})
endif ()

if (MINGW)
    target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            # need to link with mingw32 before SDL2
            mingw32
    )
endif ()

# link the libraries that we've found
if (USE_PACKAGE_MANAGER)
  if (ENABLE_SDL2_MIXER)
    target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            CONAN_PKG::sdl2
            CONAN_PKG::sdl2_image
            CONAN_PKG::sdl2_ttf
            CONAN_PKG::sdl2_mixer
            CONAN_PKG::libnoise
            CONAN_PKG::zlib
    )
  else ()
      target_link_libraries(
          ${PROJECT_NAME}
          PRIVATE
          CONAN_PKG::sdl2
          CONAN_PKG::sdl2_image
          CONAN_PKG::sdl2_ttf
          CONAN_PKG::libnoise
          CONAN_PKG::zlib
      )
  endif ()

else (USE_PACKAGE_MANAGER)

    target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            ${SDL2_LIBRARIES}
            ${SDL2_image_LIBRARIES}
            ${SDL2_ttf_LIBRARIES}
            ${SDL2_mixer_LIBRARIES}
            LIBNOISE::LIBNOISE_LIBRARIES
            ${ZLIB_LIBRARIES}
    )

    target_include_directories(
            ${PROJECT_NAME}
            PRIVATE
            ${SDL2_INCLUDE_DIRS}
            ${SDL2_image_INCLUDE_DIRS}
            ${SDL2_ttf_INCLUDE_DIRS}
            ${SDL2_mixer_INCLUDE_DIRS}
            ${ZLIB_INCLUDE_DIRS}
    )

endif (USE_PACKAGE_MANAGER)


target_compile_features(
        ${PROJECT_NAME}
        PRIVATE
        # enable C++14
        cxx_std_14
)

set (COMPILER_FLAGS VERSION="${PROJECT_NAME} ${PROJECT_VERSION} - tech preview")

if (NOT ENABLE_SDL2_MIXER)
  list(APPEND COMPILER_FLAGS DISABLE_SDL2_MIXER)
endif()

target_compile_definitions(
        ${PROJECT_NAME}
        PRIVATE
        # pass the version string via the compiler
        ${COMPILER_FLAGS}
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
    endif ()
if (BUILD_TILEDATA_EDITOR)
    add_subdirectory(src/TileDataUi)

    # QT5 is used for the TileData Editing tool
    if (USE_PACKAGE_MANAGER)
        find_package(Qt5 CONFIG COMPONENTS Core Widgets REQUIRED)
    else()
        find_package(Qt5Core CONFIG REQUIRED)
        find_package(Qt5Widgets CONFIG REQUIRED)
    endif ()# USE_PACKAGE_MANAGER
endif ()# BUILD_TILEDATA_EDITOR

# use Doxygen to generate the documentation
find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(DoxyFile_src ${PROJECT_SOURCE_DIR}/Doxyfile)
    set(DoxyFile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DoxyFile_src} ${DoxyFile} @ONLY)

    message("Use target doc to build the documentation")
    add_custom_target(
            doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DoxyFile}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            VERBATIM
    )
endif ()

if (MSVC)
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
endif ()

install(TARGETS ${PROJECT_NAME} DESTINATION .)
if (NOT APPLE)
    install(DIRECTORY resources DESTINATION .)

    if (EXISTS ${PROJECT_SOURCE_DIR}/Cytopia_Resources)
        install(DIRECTORY ${PROJECT_SOURCE_DIR}/Cytopia_Resources/music DESTINATION ./resources/audio)
    endif ()
endif ()

install(FILES ReleaseNotes.txt credits.txt DESTINATION .)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE RESOURCE TRUE)
endif ()

include(FeatureSummary)
feature_summary(WHAT ALL)
