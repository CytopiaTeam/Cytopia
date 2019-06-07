cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
include(CMakeDependentOption)

option(BUILD_TILEDATA_EDITOR "Build Cytopia TileData Editor" OFF)
option(BUILD_TEST "Build Cytopia Tests" ON)
option(USE_PACKAGE_MANAGER "Use conan for managing packages" ON)
cmake_dependent_option(USE_PMM "Use PMM for managing conan" OFF WIN32 ON)
option(ENABLE_SDL2_MIXER "Enable SDL2 Mixer" ON)
option(ENABLE_MICROPROFILE "Enable microprofile" OFF)
option(ENABLE_ANGELSCRIPT "Enable AngelScript" ON)

if (NOT BUILD_TILEDATA_EDITOR)
    set(_CC_QT "# ")
endif ()

if (NOT ENABLE_SDL2_MIXER)
    set(_CC_SDL2_MIXER "# ")
endif ()

if (NOT BUILD_TEST)
    set(_CC_CATCH2 "# ")
endif ()

if (NOT ENABLE_ANGELSCRIPT)
    set(_CC_ANGELSCRIPT "# ")
endif ()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt.in" "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt")

set(_include_directories "")
set(_compile_definitions "")
set(_link_libraries "")

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
endif ()

if (USE_PACKAGE_MANAGER)

    if (USE_PMM)
        include(cmake/pmm.cmake)

        pmm(CONAN
                REMOTES
                AFG https://api.bintray.com/conan/anotherfoxguy/conan-packages
                catchorg https://api.bintray.com/conan/catchorg/Catch2
                ror-dependencies https://api.bintray.com/conan/anotherfoxguy/ror-dependencies
                BINCRAFTERS
                )

        if (DEFINED ENV{CI})
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        endif ()
    else ()
        include(cmake/conan.cmake)

        conan_check()

        conan_add_remote(NAME AFG URL https://api.bintray.com/conan/anotherfoxguy/conan-packages)
        conan_add_remote(NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan)
        conan_add_remote(NAME catchorg URL https://api.bintray.com/conan/catchorg/Catch2)
        conan_add_remote(NAME ror-dependencies URL https://api.bintray.com/conan/anotherfoxguy/ror-dependencies)


        set(CONAN_ARGS
                BASIC_SETUP
                CMAKE_TARGETS
                )

        if (NOT DEFINED ENV{CI})
            list(APPEND CONAN_ARGS NO_OUTPUT_DIRS)
        endif ()

        conan_cmake_run(CONANFILE conanfile.txt
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
    endif ()

    find_package(LIBNOISE REQUIRED)

    if (ENABLE_ANGELSCRIPT)
        find_package(AngelScript REQUIRED)
    endif ()

    if (BUILD_TEST)
        find_package(Catch2 REQUIRED)
    endif (BUILD_TEST)

endif (USE_PACKAGE_MANAGER)

if (ENABLE_MICROPROFILE)
    set(FETCHCONTENT_QUIET OFF)
    include(FetchContent)
    FetchContent_Declare(
            microprofile
            GIT_REPOSITORY https://github.com/jonasmr/microprofile.git
    )

    FetchContent_GetProperties(microprofile)
    if (NOT microprofile_POPULATED)
        FetchContent_Populate(microprofile)
    endif ()
    set(TREAT_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "" FORCE)
endif (ENABLE_MICROPROFILE)

# get linux distribution
if (UNIX AND NOT APPLE)
    execute_process(COMMAND head -1 /etc/os-release OUTPUT_VARIABLE linux_distribution)
endif ()

# Configuration for packaging
include(cmake/CpackConfiguration.cmake)

# Compile options
include(cmake/CompileOptions.cmake)

# set install dir here for convenience
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/redist")

# make sure, every binary lands in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

message(STATUS "Building Project:  ${PROJECT_NAME}")
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE})

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()

set(SOURCES
        src/engine/basics/Camera.cxx
        src/engine/basics/isoMath.cxx
        src/engine/basics/mapEdit.cxx
        src/engine/basics/Settings.cxx
        src/engine/basics/Timer.cxx
        src/engine/basics/utils.cxx
        src/engine/GameObjects/MapNode.cxx
        src/engine/map/MapLayers.cxx
        src/engine/map/TerrainGenerator.cxx
        src/engine/ui/basics/UIElement.cxx
        src/engine/ui/basics/ButtonGroup.cxx
        src/engine/ui/basics/Layout.cxx
        src/engine/ui/widgets/Bar.cxx
        src/engine/ui/widgets/Button.cxx
        src/engine/ui/widgets/Combobox.cxx
        src/engine/ui/widgets/Checkbox.cxx
        src/engine/ui/widgets/Frame.cxx
        src/engine/ui/widgets/Image.cxx
        src/engine/ui/widgets/Text.cxx
        src/engine/ui/widgets/TextField.cxx
        src/engine/ui/widgets/Tooltip.cxx
        src/engine/ui/menuGroups/MenuGroupBuild.cxx
        src/engine/Engine.cxx
        src/engine/EventManager.cxx
        src/engine/Map.cxx
        src/engine/Sprite.cxx
        src/engine/ResourcesManager.cxx
        src/engine/TileManager.cxx
        src/engine/UIManager.cxx
        src/engine/WindowManager.cxx
        src/Game.cxx
        src/main.cxx
        )

if (ENABLE_SDL2_MIXER)
    list(APPEND SOURCES
            src/engine/audio/SoundEffect.cxx
            src/engine/audio/SoundEffect.cxx
            src/engine/audio/Music.cxx
            src/engine/AudioMixer.cxx
            )
endif ()

if (ENABLE_ANGELSCRIPT)
    list(APPEND SOURCES
            external/as_add_on/scriptbuilder/scriptbuilder.cpp
            external/as_add_on/scriptbuilder/scriptbuilder.h
            external/as_add_on/scriptstdstring/scriptstdstring.h
            external/as_add_on/scriptstdstring/scriptstdstring.cpp
            src/Scripting/ScriptEngine.cxx
            src/Scripting/ScriptEngine.hxx
            )
endif ()

if (ENABLE_MICROPROFILE)
    list(APPEND SOURCES
            ${microprofile_SOURCE_DIR}/microprofile.h
            ${microprofile_SOURCE_DIR}/microprofile_html.h
            ${microprofile_SOURCE_DIR}/microprofile.cpp
            )
endif (ENABLE_MICROPROFILE)

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

if (ENABLE_MICROPROFILE)
    list(APPEND _include_directories ${microprofile_SOURCE_DIR})
    list(APPEND _compile_definitions
            MICROPROFILE_ENABLED
            MICROPROFILE_GPU_TIMERS=0
            )
    if (WIN32)
        list(APPEND _link_libraries ws2_32.lib)
    endif (WIN32)
endif (ENABLE_MICROPROFILE)

if (MINGW)
    # need to link with mingw32 before SDL2
    list(APPEND _link_libraries mingw32)
endif ()

# link the libraries that we've found
if (USE_PACKAGE_MANAGER)
    list(APPEND _link_libraries
            CONAN_PKG::sdl2
            CONAN_PKG::sdl2_image
            CONAN_PKG::sdl2_ttf
            CONAN_PKG::libnoise
            CONAN_PKG::zlib
            )

    if (ENABLE_SDL2_MIXER)
        list(APPEND _link_libraries CONAN_PKG::sdl2_mixer)
    endif ()

    if (ENABLE_ANGELSCRIPT)
        list(APPEND _link_libraries CONAN_PKG::AngelScript)
    endif ()

    if (BUILD_TEST)
        list(APPEND _link_libraries CONAN_PKG::Catch2)
    endif ()


else (USE_PACKAGE_MANAGER)
    list(APPEND _include_directories
            ${SDL2_INCLUDE_DIRS}
            ${SDL2_image_INCLUDE_DIRS}
            ${SDL2_ttf_INCLUDE_DIRS}
            ${ZLIB_INCLUDEDIR}
            )
    list(APPEND _link_libraries
            ${SDL2_LIBRARIES}
            ${SDL2_image_LIBRARIES}
            ${SDL2_ttf_LIBRARIES}
            LIBNOISE::LIBNOISE_LIBRARIES
            ${ZLIB_LIBRARIES}
            )

    if (ENABLE_SDL2_MIXER)
        list(APPEND _link_libraries ${SDL2_mixer_LIBRARIES})
    endif ()

    if (ENABLE_ANGELSCRIPT)
        list(APPEND _link_libraries ${AngelScript_LIBRARY})
    endif ()

    if (BUILD_TEST)
        list(APPEND _link_libraries ${CATCH2_LIBRARIES})
    endif ()

endif (USE_PACKAGE_MANAGER)


# enable C++14
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

list(APPEND _compile_definitions VERSION="${PROJECT_NAME} ${PROJECT_VERSION} - tech preview")

if (ENABLE_SDL2_MIXER)
    list(APPEND _compile_definitions USE_SDL2_MIXER)
endif ()

if (ENABLE_ANGELSCRIPT)
    list(APPEND _compile_definitions USE_ANGELSCRIPT)
    list(APPEND _include_directories external/as_add_on)
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE ${_include_directories})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${_compile_definitions})
target_link_libraries(${PROJECT_NAME} PRIVATE ${_link_libraries})

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
    else ()
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

if (BUILD_TEST)
    include(tests/CMakeLists.txt)
endif (BUILD_TEST)

include(FeatureSummary)
feature_summary(WHAT ALL)
