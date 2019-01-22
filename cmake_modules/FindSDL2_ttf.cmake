# This module searches for the SDL2_ttf library and defines:
#
# SDL2_ttf_FOUND - if false, do not try to link
#
# It provides one library in the SDL namespace:
#
# SDL::SDL2_ttf - Link if you are creating a library.

# Find the include directory where SDL_ttf.h is located.
find_path(
  SDL2_TTF_INCLUDE_DIR
  NAMES SDL_ttf.h
  PATH_SUFFIXES include include/sdl2 include/SDL2 SDL2
  HINTS ${SDL2_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/ThirdParty/sdl
)

if(WIN32 AND NOT MINGW)
  # On windows we give CMake a little more guidance for 32 vs. 64 bit paths.
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SDL2_LIBRARY_PATH_SUFFIX lib/x64)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(SDL2_LIBRARY_PATH_SUFFIX lib/x86)
  endif()
endif()

# Find the font library for SDL2.
find_library(
  SDL2_TTF_LIBRARY
  NAMES SDL2_ttf
  PATH_SUFFIXES ${SDL2_LIBRARY_PATH_SUFFIX}
  HINTS ${SDL2_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/ThirdParty/sdl
)

if(SDL2_TTF_INCLUDE_DIR AND SDL2_TTF_LIBRARY)
  add_library(SDL::SDL2_ttf UNKNOWN IMPORTED)
  set_target_properties(
    SDL::SDL2_ttf PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_DIR}"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION "${SDL2_TTF_LIBRARY}"
  )
endif()

# Ensure that everything was found correctly.
# This will also set SDL2_ttf_FOUND to true/false.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  SDL2_ttf
  REQUIRED_VARS
    SDL2_TTF_LIBRARY
    SDL2_TTF_INCLUDE_DIR
)

mark_as_advanced(
  SDL2_TTF_INCLUDE_DIR
  SDL2_TTF_LIBRARY
)