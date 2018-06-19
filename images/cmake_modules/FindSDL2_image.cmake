# This module searches for the SDL2_image library and defines:
#
# SDL2_image_FOUND - if false, do not try to link
#
# It provides one library in the SDL namespace:
#
# SDL::SDL2_image - Link if you are creating a library.

# Find the include directory where SDL_image.h is located.
find_path(
  SDL2_IMAGE_INCLUDE_DIR
  NAMES SDL_image.h
  PATH_SUFFIXES SDL2
)

if(WIN32 AND NOT MINGW)
  # On windows we give CMake a little more guidance for 32 vs. 64 bit paths.
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SDL2_LIBRARY_PATH_SUFFIX lib/x64)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(SDL2_LIBRARY_PATH_SUFFIX lib/x86)
  endif()
endif()

# Find the image library for SDL2.
find_library(
  SDL2_IMAGE_LIBRARY
  NAMES SDL2_image
  PATH_SUFFIXES ${SDL2_LIBRARY_PATH_SUFFIX}
)

if(SDL2_IMAGE_INCLUDE_DIR AND SDL2_IMAGE_LIBRARY)
  add_library(SDL::SDL2_image UNKNOWN IMPORTED)
  set_target_properties(
    SDL::SDL2_image PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIR}"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION "${SDL2_IMAGE_LIBRARY}"
  )
endif()

# Ensure that everything was found correctly.
# This will also set SDL2_image_FOUND to true/false.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  SDL2_image
  REQUIRED_VARS SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR
)

mark_as_advanced(
  SDL2_IMAGE_INCLUDE_DIR
  SDL2_IMAGE_LIBRARY
)