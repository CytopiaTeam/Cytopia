cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/find-modules")
include(CMakeDependentOption)
include(Macros)
include(FeatureSummary)
include(SourceFileUtils)
set(_PREFIX "CYTOPIA_")

# Test if conan is installed
find_program(CONAN_CMD_TEST conan NO_CACHE)
cmake_dependent_option(USE_PACKAGE_MANAGER "Use conan" ON "NOT CONAN_CMD_TEST STREQUAL CONAN_CMD_TEST-NOTFOUND" OFF)
message(STATUS "Using conan: ${USE_PACKAGE_MANAGER}")

# create the cmake project
project(
        # this will be the ${PROJECT_NAME}
        Cytopia
        # this will provide ${PROJECT_VERSION}
        VERSION 0.2.1
        # Description
        DESCRIPTION "A city-building game"
        # we are using C++
        LANGUAGES CXX
)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_program(SCCACHE_PROGRAM sccache)
find_program(CCACHE_PROGRAM ccache)
if (SCCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}")
    message("Using ${SCCACHE_PROGRAM} for speeding up build")
elseif (CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message("Using ${CCACHE_PROGRAM} for speeding up build")
endif ()


# Allow user to globally set the library preference for external libraries
cmd_option(
        ${_PREFIX}LIB_PREFERENCE
        "Library preference [SYSTEM (if available), CONAN]"
        "CONAN"
        STRINGS "SYSTEM" "CONAN"
)

# Special mode, that will force dependencies to the packages provided by system unless they were set to conan explicitly.
option(${_PREFIX}FORCE_SYSTEM_DEPENDENCIES "Force the use of system packages")

option(BUILD_TEST "Build Cytopia Tests" ON)
option(ENABLE_DEBUG "Enable Debug (asserts and logs)" OFF)
option(BUILD_ANDROID "Crossbuild to Android" OFF)

# setup paths
set(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")
set(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")
set(EXECUTABLE_OUTPUT_PATH ${RUNTIME_OUTPUT_DIRECTORY})
set(LIBRARY_OUTPUT_PATH ${RUNTIME_OUTPUT_DIRECTORY})

if (WIN32)
    set(CMAKE_CONFIGURATION_TYPES "Debug" "Release" CACHE STRING "Configuration types")
    # fix executable paths for windows
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME_OUTPUT_DIRECTORY})
endif ()

set(_include_directories "${CMAKE_CURRENT_SOURCE_DIR}/external/header_only")
set(_compile_definitions "")
set(_link_libraries "")

if (BUILD_ANDROID)
    message(STATUS "Crossbuilding to Android")
    # Set some helper variables.
    string(TOLOWER "${CMAKE_SYSTEM_NAME}" targetSystem)
    set(projectDir "${CMAKE_CURRENT_LIST_DIR}")
    set(sourceDir "${projectDir}/source")
    set(targetName "SimpleExample")
    set(binDir "${projectDir}/bin")

    # Define executable output dir.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${binDir}/${targetSystem}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${binDir}/${targetSystem}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${binDir}/${targetSystem}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${binDir}/${targetSystem}_debug")

    find_library(OpenGLES2_LIBRARY GLESv2 REQUIRED)
    find_library(OpenGLES3_LIBRARY GLESv3 REQUIRED)
    find_library(ANDROID_LOG_LIBRARY log REQUIRED)
    find_library(ANDROID_LIBRARY_LIBRARY android REQUIRED)
    find_library(zlib z)

    set(DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/platforms/android/build/SDL2/intermediates/ndkBuild)
    set(NOISE_DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/platforms/android/build/libnoise/intermediates/cmake)
    set(OPENAL_DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/platforms/android/build/openal-soft/intermediates/cmake)
    set(LIBOGG_DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/platforms/android/build/libogg/intermediates/cmake)
    set(LIBVORBIS_DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/platforms/android/build/libvorbis/intermediates/cmake)
    set(LIBPNG_DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/platforms/android/build/libpng/intermediates/cmake)
    set(ZLIB_DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/platforms/android/build/zlib/intermediates/cmake)
    string(TOLOWER ${CMAKE_BUILD_TYPE} ANDROID_BUILD_DIR)

    set(SDL_LOCATION ${CMAKE_SOURCE_DIR}/contrib/)

    ##SDL2
    set(SDL2_LOCATION ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2.so)
    add_library(SDL2 SHARED IMPORTED)
    set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${SDL2_LOCATION})

    ## SDL2_image
    set(SDL2_IMAGE_LOCATION ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_image.so)
    add_library(SDL2_IMAGE SHARED IMPORTED)
    set_target_properties(SDL2_IMAGE PROPERTIES IMPORTED_LOCATION ${SDL2_IMAGE_LOCATION})

    ## SDL2_ttf
    set(SDL2_TTF_LOCATION ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_ttf.so)
    add_library(SDL2_TTF SHARED IMPORTED)
    set_target_properties(SDL2_TTF PROPERTIES IMPORTED_LOCATION ${SDL2_TTF_LOCATION})

    #libnoise
    set(LIBNOISE_LOCATION ${NOISE_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libnoise.so)
    add_library(LIBNOISE SHARED IMPORTED)
    set_target_properties(LIBNOISE PROPERTIES IMPORTED_LOCATION ${LIBNOISE_LOCATION})

    #openal
    set(OPENAL_LOCATION ${OPENAL_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libopenal.so)
    add_library(OPENAL SHARED IMPORTED)
    set_target_properties(OPENAL PROPERTIES IMPORTED_LOCATION ${OPENAL_LOCATION})

    #ogg
    set(LIBOGG_LOCATION ${LIBOGG_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libogg.so)
    add_library(LIBOGG SHARED IMPORTED)
    set_target_properties(LIBOGG PROPERTIES IMPORTED_LOCATION ${LIBOGG_LOCATION})

    #zlib ./build/zlib/intermediates/cmake/debug/obj/x86/zlib.so
    set(ZLIB_LOCATION ${ZLIB_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/zlib.so)
    add_library(ZLIB SHARED IMPORTED)
    set_target_properties(ZLIB PROPERTIES IMPORTED_LOCATION ${ZLIB_LOCATION})

    #libpng ./build/libpng/intermediates/cmake/debug/obj/x86/libpng.so
    set(LIBPNG_LOCATION ${LIBPNG_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libpng.so)
    add_library(LIBPNG SHARED IMPORTED)
    set_target_properties(LIBPNG PROPERTIES IMPORTED_LOCATION ${LIBPNG_LOCATION})

    #vorbis ./build/libvorbis/intermediates/cmake/debug/obj/x86/libvorbis.so
    set(LIBVORBIS_LOCATION ${LIBVORBIS_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libvorbis.so)
    add_library(LIBVORBIS SHARED IMPORTED)
    set_target_properties(LIBVORBIS PROPERTIES IMPORTED_LOCATION ${LIBVORBIS_LOCATION})

    set(LIBVORBISFILE_LOCATION ${LIBVORBIS_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libvorbisfile.so)
    add_library(LIBVORBISFILE SHARED IMPORTED)
    set_target_properties(LIBVORBISFILE PROPERTIES IMPORTED_LOCATION ${LIBVORBISFILE_LOCATION})
    # Joysticks were causing trouble on android.
    add_definitions(-DSDL_JOYSTICK=0)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    list(APPEND _compile_definitions DEBUG)
endif ()


################################################################################
# Check for dependencies
################################################################################
include(DependenciesConfig)
resolve_conan_dependencies()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    link_libraries(dbghelp.lib)
elseif (NOT APPLE AND NOT BUILD_ANDROID)
    # Note: Apple supports std::filesystem with macOS10.15 and upwards.
    # Needed for filesystem library
    list(APPEND _link_libraries "stdc++fs")
endif ()

if (MINGW)
    # need to link with mingw32 before SDL2
    list(APPEND _link_libraries mingw32)
endif ()


# get linux distribution
if (UNIX AND NOT APPLE)
    execute_process(COMMAND head -1 /etc/os-release OUTPUT_VARIABLE linux_distribution)
endif ()

# Configuration for packaging
include(CpackConfiguration)

# Compile options
include(CompileOptions)

# set install dir here for convenience
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/redist")

message(STATUS "Building Project:  ${PROJECT_NAME}")
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE})

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fdiagnostics-color=always")
endif ()

add_subdirectory(src)

if (BUILD_TEST)
    include(CTest)
    enable_testing()

    add_subdirectory(tests)
endif (BUILD_TEST)

if (NOT BUILD_ANDROID)
    # use Doxygen to generate the documentation
    find_package(Doxygen)
endif ()

if (DOXYGEN_FOUND)
    add_custom_target(
            doc
            COMMAND "${DOXYGEN_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/Doxyfile"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            VERBATIM
    )
endif ()

# copy itch.toml to build folder to install our prerequisits
if (WIN32)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8) ## 64 bit
        message(STATUS "Using Win64 itch.toml file")
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/tools/itch/win64/.itch.toml
                DESTINATION ${CMAKE_BINARY_DIR}/redist
                )
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4) ## 32 bit
        message(STATUS "Using Win64 itch.toml file")
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/tools/itch/win32/.itch.toml
                DESTINATION ${CMAKE_BINARY_DIR}/redist
                )
    endif ()
endif ()
