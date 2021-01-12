cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
# add the path to our custom find modules

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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
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

include(CMakeDependentOption)
include(SourceFileUtils)

set(PACKAGE_MANAGER "conan" CACHE STRING "Use conan for managing packages")
set_property(CACHE PACKAGE_MANAGER PROPERTY STRINGS conan pkgconfig none)
option(BUILD_TEST "Build Cytopia Tests" ON)
option(ENABLE_AUDIO "Enable Audio" ON)
option(ENABLE_MICROPROFILE "Enable microprofile" OFF)
option(ENABLE_ANGELSCRIPT "Enable AngelScript" ON)
option(ENABLE_VULKAN "Enable Vulkan" OFF)
option(ENABLE_DEBUG "Enable Debug (asserts and logs)" OFF)
option(BUILD_ANDROID "Crossbuild to Android" OFF)

# Comment-out uneeded libs
if (NOT ENABLE_AUDIO)
    set(_CC_OPENAL_SOFT "# ")
endif ()

if (NOT ENABLE_ANGELSCRIPT)
    set(_CC_ANGELSCRIPT "# ")
endif ()

if (NOT ENABLE_VULKAN)
  set(_CC_VULKAN "# ")
endif ()

if(PACKAGE_MANAGER STREQUAL "conan")
  if (WIN32)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conanfile-win.txt.in" "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt")
  elseif(APPLE)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conanfile-mac.txt.in" "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt")
  else()
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conanfile-linux.txt.in" "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt")
  endif ()
endif()

set(_include_directories "${CMAKE_CURRENT_SOURCE_DIR}/external/header_only")
set(_compile_definitions "")
set(_link_libraries "")

if (BUILD_ANDROID)
  message (STATUS "Crossbuilding to Android")
  # Set some helper variables.
  string(TOLOWER "${CMAKE_SYSTEM_NAME}" targetSystem)
  set(projectDir  "${CMAKE_CURRENT_LIST_DIR}")
  set(sourceDir   "${projectDir}/source")
  set(targetName  "SimpleExample")
  set(binDir      "${projectDir}/bin")

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
  set (SDL2_LOCATION ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2.so)
  add_library(SDL2 SHARED IMPORTED )
  set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${SDL2_LOCATION})

  ## SDL2_image
  set (SDL2_IMAGE_LOCATION ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_image.so)
  add_library(SDL2_IMAGE SHARED IMPORTED )
  set_target_properties(SDL2_IMAGE PROPERTIES IMPORTED_LOCATION ${SDL2_IMAGE_LOCATION})

  ## SDL2_ttf
  set (SDL2_TTF_LOCATION ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_ttf.so)
  add_library(SDL2_TTF SHARED IMPORTED )
  set_target_properties(SDL2_TTF PROPERTIES IMPORTED_LOCATION ${SDL2_TTF_LOCATION})

  #libnoise
  set (LIBNOISE_LOCATION ${NOISE_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libnoise.so)
  add_library(LIBNOISE SHARED IMPORTED )
  set_target_properties(LIBNOISE PROPERTIES IMPORTED_LOCATION ${LIBNOISE_LOCATION})

  #openal
  set (OPENAL_LOCATION ${OPENAL_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libopenal.so)
  add_library(OPENAL SHARED IMPORTED )
  set_target_properties(OPENAL PROPERTIES IMPORTED_LOCATION ${OPENAL_LOCATION})

  #ogg
  set (LIBOGG_LOCATION ${LIBOGG_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libogg.so)
  add_library(LIBOGG SHARED IMPORTED )
  set_target_properties(LIBOGG PROPERTIES IMPORTED_LOCATION ${LIBOGG_LOCATION})
  
  #zlib ./build/zlib/intermediates/cmake/debug/obj/x86/zlib.so
  set (ZLIB_LOCATION ${ZLIB_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/zlib.so)
  add_library(ZLIB SHARED IMPORTED )
  set_target_properties(ZLIB PROPERTIES IMPORTED_LOCATION ${ZLIB_LOCATION})
  
  #libpng ./build/libpng/intermediates/cmake/debug/obj/x86/libpng.so
  set (LIBPNG_LOCATION ${LIBPNG_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libpng.so)
  add_library(LIBPNG SHARED IMPORTED )
  set_target_properties(LIBPNG PROPERTIES IMPORTED_LOCATION ${LIBPNG_LOCATION})

  #vorbis ./build/libvorbis/intermediates/cmake/debug/obj/x86/libvorbis.so
  set (LIBVORBIS_LOCATION ${LIBVORBIS_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libvorbis.so)
  add_library(LIBVORBIS SHARED IMPORTED )
  set_target_properties(LIBVORBIS PROPERTIES IMPORTED_LOCATION ${LIBVORBIS_LOCATION})

  set (LIBVORBISFILE_LOCATION ${LIBVORBIS_DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/${ANDROID_ABI}/libvorbisfile.so)
  add_library(LIBVORBISFILE SHARED IMPORTED )
  set_target_properties(LIBVORBISFILE PROPERTIES IMPORTED_LOCATION ${LIBVORBISFILE_LOCATION})
  # Joysticks were causing trouble on android.
  add_definitions(-DSDL_JOYSTICK=0)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  list(APPEND _compile_definitions DEBUG)
endif ()

if (APPLE AND NOT (PACKAGE_MANAGER STREQUAL "conan"))
  # on macOS, usr/local/lib is not in the default search path for libraries. Homebrew install it's libraries there though.
  LINK_DIRECTORIES(${LINK_DIRECTORIES} /usr/local/lib)
endif()


if (PACKAGE_MANAGER STREQUAL "conan")
    function(get_variable_from_conan _var_name)
        include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
        get_property(__propt VARIABLE PROPERTY ${_var_name})
        SET(${_var_name} ${__propt} PARENT_SCOPE)
    endfunction()

    include(pmm)

    pmm(CONAN
			REMOTES
            AFG https://api.bintray.com/conan/anotherfoxguy/conan-packages
            ror-dependencies https://api.bintray.com/conan/anotherfoxguy/ror-dependencies
            cytopia https://api.bintray.com/conan/cytopia/conan-packages
            BINCRAFTERS
            CMakeCM ROLLING
            )

    if (DEFINED ENV{CI})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif ()


    list(APPEND _link_libraries
            CONAN_PKG::sdl2
            CONAN_PKG::sdl2_image
            CONAN_PKG::sdl2_ttf
            CONAN_PKG::sdl2_ttf
            CONAN_PKG::libnoise
            CONAN_PKG::zlib
            )

    if (ENABLE_AUDIO)
        list(APPEND _link_libraries CONAN_PKG::vorbis)
        list(APPEND _link_libraries CONAN_PKG::openal)
    endif ()

    if (ENABLE_ANGELSCRIPT)
        list(APPEND _link_libraries CONAN_PKG::AngelScript)
    endif ()

    if (ENABLE_VULKAN)
        list(APPEND _link_libraries CONAN_PKG::vulkan-sdk)
    endif()

elseif (BUILD_ANDROID)
    list(APPEND _link_libraries ${OpenGLES2_LIBRARY}
                                ${ANDROID_LOG_LIBRARY}
                                ${ANDROID_LIBRARY_LIBRARY}
                                SDL2
                                SDL2_IMAGE
                                SDL2_TTF
                                OPENAL
                                LIBVORBIS
                                LIBVORBISFILE
                                LIBOGG
                                z # zlib provided by android NDK
                                LIBNOISE
                                LIBPNG

    )
    # Include dirs.
    list(APPEND _include_directories ${CMAKE_SOURCE_DIR}/external/android/SDL/SDL2/include
                                     ${CMAKE_SOURCE_DIR}/external/android/SDL/SDL2_image
                                     ${CMAKE_SOURCE_DIR}/external/android/SDL/SDL2_ttf
                                     ${CMAKE_SOURCE_DIR}/external/android/other/libnoise/src/noise
                                     ${CMAKE_SOURCE_DIR}/external/android/other/openal-soft/include
                                     ${CMAKE_SOURCE_DIR}/external/android/other/ogg/include
                                     ${CMAKE_SOURCE_DIR}/external/android/other/vorbis/include
                                     ${CMAKE_SOURCE_DIR}/external/android/other/libpng/
        )
elseif(PACKAGE_MANAGER STREQUAL "pkgconfig")
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_image REQUIRED SDL2_image)
    pkg_check_modules(SDL2_ttf REQUIRED SDL2_ttf)
    pkg_check_modules(ZLIB REQUIRED zlib)
    pkg_check_modules(PNG REQUIRED libpng)

    if (ENABLE_AUDIO)
        pkg_check_modules(openal REQUIRED openal)
        pkg_check_modules(vorbis REQUIRED vorbis)
        pkg_check_modules(vorbisfile REQUIRED vorbisfile)
    endif ()

    if (ENABLE_VULKAN)
        pkg_check_modules(VULKAN REQUIRED vulkan)
    endif()

    find_package(LIBNOISE REQUIRED)

    if (ENABLE_ANGELSCRIPT)
        find_package(AngelScript REQUIRED)
    endif ()

    list(APPEND _include_directories
            ${SDL2_INCLUDE_DIRS}
            ${SDL2_image_INCLUDE_DIRS}
            ${SDL2_ttf_INCLUDE_DIRS}
            ${ZLIB_INCLUDEDIR}
            ${PNG_INCLUDE_DIRS}
            )
    list(APPEND _link_libraries
            ${SDL2_LIBRARIES}
            ${SDL2_image_LIBRARIES}
            ${SDL2_ttf_LIBRARIES}
            LIBNOISE::LIBNOISE_LIBRARIES
            ${ZLIB_LIBRARIES}
            ${PNG_LIBRARIES}
            )

    if (ENABLE_AUDIO)
        list(APPEND _link_libraries ${vorbis_LIBRARIES})
        list(APPEND _link_libraries ${vorbisfile_LIBRARIES})
    endif ()

    if (ENABLE_AUDIO)
        list(APPEND _link_libraries ${openal_LIBRARIES})
    endif ()


    if (ENABLE_ANGELSCRIPT)
        list(APPEND _link_libraries ${AngelScript_LIBRARY})
    endif ()

    if (ENABLE_VULKAN)
        list(APPEND _link_libraries ${VULKAN_LIBRARIES})
        list(APPEND _include_directories ${VULKAN_INCLUDEDIR})
    endif()

elseif(PACKAGE_MANAGER STREQUAL "none")
    macro(find_dependency _dep_name)
      set("${_dep_name}_INCLUDE_DIRS" "Change me" CACHE PATH "Include directory for ${_dep_name}")
      set("${_dep_name}_LIBRARIES" "Change me" CACHE PATH "Library binaries directory for ${_dep_name}")
        if(NOT IS_DIRECTORY "${${_dep_name}_INCLUDE_DIRS}")
          message(SEND_ERROR "For dependency ${_dep_name}: \"${${_dep_name}_INCLUDE_DIRS}\" is not a directory")
        elseif(NOT IS_DIRECTORY "${${_dep_name}_LIBRARIES}")
          message(SEND_ERROR "For dependency ${_dep_name}: \"${${_dep_name}\"_LIBRARIES} is not a directory")
        else()
          list(APPEND _include_directories ${${_dep_name}_INCLUDE_DIRS})
          list(APPEND _link_libraries ${${_dep_name}_LIBRARY})
          return()
        endif()
        list(APPEND _DEPENDENCY_ERRORS "${_dep_name}")
    endmacro()

    find_dependency(SDL2)
    find_dependency(SDL2_image)
    find_dependency(SDL2_ttf)
    find_dependency(ZLIB)
    find_dependency(LIBNOISE)
    find_dependency(LIBPNG)

    if (ENABLE_AUDIO)
        find_dependency(openal)
        find_dependency(vorbis)
        find_dependency(vorbisfile)
    endif ()

    if (ENABLE_ANGELSCRIPT)
        find_dependency(AngelScript)
    endif ()

    list(LENGTH _DEPENDENCY_ERRORS _ERROR_LENGTH)
    if(_ERROR_LENGTH)
      message(FATAL_ERROR "Could not find ${_ERROR_LENGTH} dependencies: ${_DEPENDENCY_ERRORS}")
    endif ()
endif ()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    link_libraries(dbghelp.lib)
elseif(NOT APPLE AND NOT BUILD_ANDROID)
    # Note: Apple supports std::filesystem with macOS10.15 and upwards.
    # Needed for filesystem library
    list(APPEND _link_libraries "stdc++fs")
endif()

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

if (ENABLE_AUDIO)
  list(APPEND _compile_definitions USE_AUDIO)
endif ()


if (ENABLE_ANGELSCRIPT)
    list(APPEND _compile_definitions USE_ANGELSCRIPT)
    list(APPEND _include_directories ${CMAKE_SOURCE_DIR}/external/as_add_on)
endif ()

if (ENABLE_MICROPROFILE)
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
include(CpackConfiguration)

# Compile options
include(CompileOptions)

# set install dir here for convenience
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/redist")

# make sure, every binary lands in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

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
endif()

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
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4) ## 32 bit
    message(STATUS "Using Win64 itch.toml file")
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/tools/itch/win32/.itch.toml
           DESTINATION ${CMAKE_BINARY_DIR}/redist
           )
  endif()
endif()

include(FeatureSummary)
feature_summary(WHAT ALL)
