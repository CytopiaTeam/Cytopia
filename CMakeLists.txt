cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/find-modules")
include(CMakeDependentOption)
include(Macros)
include(FeatureSummary)
include(SourceFileUtils)
include(CMakeCM)
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
        VERSION 0.4
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
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behaviour Sanitizer" OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)

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

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    list(APPEND _compile_definitions DEBUG)
endif ()

################################################################################
# Check for dependencies
################################################################################

if (ANDROID)
    message(STATUS "Crossbuilding to Android")
else ()
    include(DependenciesConfig)
    resolve_conan_dependencies()
endif ()


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
