cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/find-modules" "${CMAKE_SOURCE_DIR}/cmake/dependencies-helpers")

cmake_policy(SET CMP0127 OLD)

include(FindPkgConfig)
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH TRUE)
include(CMakeDependentOption)
include(Macros)
include(FeatureSummary)
include(SourceFileUtils)
include(CMakeCM)

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

option(BUILD_TEST "Build Cytopia Tests" ON)
option(ENABLE_PHC "Use a Precompiled header for speeding up the build" ON)
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
include(DependenciesConfig)

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

message(STATUS "Building Project:  ${PROJECT_NAME}")
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE})

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fdiagnostics-color=always")
endif ()

# Include imgui project
add_subdirectory(external/imgui)

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


feature_summary(WHAT ALL)