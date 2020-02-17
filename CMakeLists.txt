cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
# add the path to our custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMakeDependentOption)
include(SourceFileUtils)
include(FetchContent)

option(BUILD_TEST "Build Cytopia Tests" ON)
option(USE_PACKAGE_MANAGER "Use conan for managing packages" ON)
option(ENABLE_AUDIO "Enable Audio" ON)
option(ENABLE_MICROPROFILE "Enable microprofile" OFF)
option(ENABLE_ANGELSCRIPT "Enable AngelScript" ON)
option(ENABLE_MOFILEREADER "Enable MofileReader" ON)
option(CROSSBUILD_EMSCRIPTEN "Build for the EmScripten Platform" OFF)

if (CROSSBUILD_EMSCRIPTEN)
  # Turn off not features that are not compatible with emscripten
  set (ENABLE_AUDIO OFF) 
  set (ENABLE_ANGELSCRIPT OFF) 
  set (ENABLE_MOFILEREADER OFF) 
  #include(~/.conan/data/emsdk_installer/1.39.6/bincrafters/stable/package/1492a59deb6efdf29776a5734de63fc5f5c58650/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  find_package(OpenGL REQUIRED)
  include_directories(${OPENGL_INCLUDE_DIR})

 list( APPEND conan_cmake_run_params SETTINGS os.threads=true )

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --no-check-features")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --no-check-features")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --no-check-features")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -s USE_PTHREADS=1 -s USE_SDL_IMAGE=2  -s USE_SDL_TTF=2 -s USE_ZLIB=1"  )
  set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pthread -s USE_PTHREADS=1 -s USE_SDL_IMAGE=2  -s USE_SDL_TTF=2 -s USE_ZLIB=1"  )
  set(SDL2_LIBRARIES "-s USE_SDL=2")
  set(SDL2_IMAGE_LIBRARIES "-s USE_PTHREADS=1 -s USE_SDL_IMAGE=2")
  #set(SDL2_MIXER_LIBRARIES "-s USE_SDL_MIXER=2")
  set(SDL2_TTF_LIBRARIES "--no-check-features -s USE_SDL_TTF=2")
  set(SDL2_ZLIB_LIBRARIES "-s USE_ZLIB=1")
  
endif()


# Comment-out uneeded libs
if (NOT ENABLE_AUDIO)
    set(_CC_SDL2_MIXER "# ")
endif ()

if (NOT ENABLE_AUDIO)
    set(_CC_OPENAL_SOFT "# ")
endif ()

if (NOT BUILD_TEST)
    set(_CC_CATCH2 "# ")
endif ()

if (NOT ENABLE_ANGELSCRIPT)
    set(_CC_ANGELSCRIPT "# ")
endif ()

if (NOT ENABLE_MOFILEREADER)
    set(_CC_MOFILEREADER "# ")
endif ()

if (WIN32 OR APPLE)
    set(_CC_WIN32 "# ")
endif ()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt.in" "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt")

set(_include_directories "${CMAKE_CURRENT_SOURCE_DIR}/external/header_only")
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

    function(get_variable_from_conan _var_name)
        include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
        get_property(__propt VARIABLE PROPERTY ${_var_name})
        SET(${_var_name} ${__propt} PARENT_SCOPE)
    endfunction()

    include(pmm)

    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")

      pmm(CONAN
        PROFILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan_profiles/emscripten_profile"
        REMOTES
        AFG https://api.bintray.com/conan/anotherfoxguy/conan-packages
        catchorg https://api.bintray.com/conan/catchorg/Catch2
        ror-dependencies https://api.bintray.com/conan/anotherfoxguy/ror-dependencies
        BINCRAFTERS
        CMakeCM ROLLING
      )
    else()
      pmm(CONAN
              REMOTES
              AFG https://api.bintray.com/conan/anotherfoxguy/conan-packages
              catchorg https://api.bintray.com/conan/catchorg/Catch2
              ror-dependencies https://api.bintray.com/conan/anotherfoxguy/ror-dependencies
              BINCRAFTERS
              CMakeCM ROLLING
              )
    endif()

    if (DEFINED ENV{CI})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif ()


    if (BUILD_TEST)
        get_variable_from_conan(CONAN_LIB_DIRS_CATCH2)
        list(APPEND CMAKE_MODULE_PATH "${CONAN_LIB_DIRS_CATCH2}/cmake/Catch2")
    endif ()

    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    list(APPEND _link_libraries
#    "-s ERROR_ON_UNDEFINED_SYMBOLS=0"
        "-s USE_PTHREADS=1"
        "-s WASM=1"
        "-o index.html"
        CONAN_PKG::libnoise
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2TTF_LIBRARIES}
        ${SDL2_ZLIB_LIBRARIES}
    )
    else()
    list(APPEND _link_libraries
            CONAN_PKG::sdl2
            CONAN_PKG::sdl2_image
            CONAN_PKG::sdl2_ttf
            CONAN_PKG::sdl2_ttf
            CONAN_PKG::libnoise
            CONAN_PKG::zlib
            )
            endif()

    if (ENABLE_AUDIO)
        list(APPEND _link_libraries CONAN_PKG::sdl2_mixer)
    endif ()
    
    if (ENABLE_AUDIO)
        list(APPEND _link_libraries CONAN_PKG::openal)
    endif ()

    if (ENABLE_ANGELSCRIPT)
        list(APPEND _link_libraries CONAN_PKG::AngelScript)
    endif ()

    if (ENABLE_MOFILEREADER)
        list(APPEND _link_libraries CONAN_PKG::MofileReader)
    endif ()

    if (BUILD_TEST)
        list(APPEND _link_libraries CONAN_PKG::Catch2)
    endif ()

else (USE_PACKAGE_MANAGER)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_image REQUIRED SDL2_image)
    pkg_check_modules(SDL2_ttf REQUIRED SDL2_ttf)
    pkg_check_modules(ZLIB REQUIRED zlib)

    if (ENABLE_AUDIO)
        pkg_check_modules(SDL2_mixer REQUIRED SDL2_mixer)
    endif ()
    
    if (ENABLE_AUDIO)
        pkg_check_modules(openal REQUIRED openal)
    endif ()

    find_package(LIBNOISE REQUIRED)

    if (ENABLE_ANGELSCRIPT)
        find_package(AngelScript REQUIRED)
    endif ()

    if (ENABLE_MOFILEREADER)
        find_package(MofileReader REQUIRED)
    endif ()

    if (BUILD_TEST)
        find_package(Catch2 REQUIRED)
    endif (BUILD_TEST)

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

    if (ENABLE_AUDIO)
        list(APPEND _link_libraries ${SDL2_mixer_LIBRARIES})
    endif ()
    
    if (ENABLE_AUDIO)
        list(APPEND _link_libraries ${openal_LIBRARIES})
    endif ()


    if (ENABLE_ANGELSCRIPT)
        list(APPEND _link_libraries ${AngelScript_LIBRARY})
    endif ()

    if (BUILD_TEST)
        list(APPEND _link_libraries ${CATCH2_LIBRARIES})
    endif ()

endif (USE_PACKAGE_MANAGER)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  add_compile_definitions(DEBUG)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  link_libraries(dbghelp.lib)
else()
  # Needed for filesystem library
  if(NOT APPLE AND NOT EMSCRIPTEN)
    # TODO: Link with stdc++fs on Apple with XCode11
    list(APPEND _link_libraries "stdc++fs")
  endif()
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

if (ENABLE_AUDIO)
    list(APPEND _compile_definitions USE_OPENAL_SOFT)
endif (ENABLE_AUDIO)

if (ENABLE_ANGELSCRIPT)
    list(APPEND _compile_definitions USE_ANGELSCRIPT)
    list(APPEND _include_directories ${CMAKE_SOURCE_DIR}/external/as_add_on)
endif ()

if (ENABLE_MOFILEREADER)
    list(APPEND _compile_definitions USE_MOFILEREADER)
endif ()

if (ENABLE_MICROPROFILE)
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

# use Doxygen to generate the documentation
find_package(Doxygen)

if (DOXYGEN_FOUND)
    add_custom_target(
            doc
            COMMAND "${DOXYGEN_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/Doxyfile"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            VERBATIM
    )
endif ()

include(FeatureSummary)
feature_summary(WHAT ALL)
