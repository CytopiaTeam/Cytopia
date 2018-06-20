# This module searches for the SDL2 library and defines:
#
# SDL2_FOUND - if false, do not try to link
#
# It provides two libraries in the SDL namespace:
#
# SDL::SDL2 - Link if you are creating a library.
# SDL::SDL2main - Link if you are creating an executable.

# Find the include directory where the SDL2 folder is located.
find_path(
  SDL2_INCLUDE_DIR
  NAMES SDL.h
  PATH_SUFFIXES SDL2
)

if(WIN32 AND NOT MINGW)
  # On windows we give CMake a little more guidance for 32 vs. 64 bit paths
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SDL2_LIBRARY_PATH_SUFFIX lib/x64)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(SDL2_LIBRARY_PATH_SUFFIX lib/x86)
  endif()
endif()

# Find the core library for SDL2.
find_library(
  SDL2_LIBRARY
  NAMES SDL2
  PATH_SUFFIXES ${SDL2_LIBRARY_PATH_SUFFIX}
)

# Find the SDL2 library that should be linked to by executables.
find_library(
  SDL2_MAIN_LIBRARY
  NAMES SDL2main
  PATH_SUFFIXES ${SDL2_LIBRARY_PATH_SUFFIX}
)

if(SDL2_INCLUDE_DIR AND SDL2_LIBRARY AND SDL2_MAIN_LIBRARY)
  # Create an imported library for just the core SDL2 library.
  add_library(SDL::SDL2 UNKNOWN IMPORTED)
  set_target_properties(
    SDL::SDL2 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION "${SDL2_LIBRARY}"
  )

  # Create an imported library for SDL2main that depends on the core SDL2 library.
  add_library(SDL::SDL2main UNKNOWN IMPORTED)
  set_target_properties(
    SDL::SDL2main PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION "${SDL2_MAIN_LIBRARY}"
    INTERFACE_LINK_LIBRARIES "${SDL2_LIBRARY}"
  )
endif()

# Ensure that everything was found correctly.
# This will also set SDL2_FOUND to true/false.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  SDL2
  REQUIRED_VARS SDL2_LIBRARY SDL2_INCLUDE_DIR SDL2_MAIN_LIBRARY
)

mark_as_advanced(
  SDL2_INCLUDE_DIR
  SDL2_LIBRARY
  SDL2_MAIN_LIBRARY
)
