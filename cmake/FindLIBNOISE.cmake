# This module searches for the libNoise library and defines:
#
# LIBNOISE_FOUND - if false, do not try to link
#
# This module defines
# LIBNOISE::LIBNOISE_LIBRARIES
# LIBNOISE::LIBNOISE_INCLUDE_DIR, where to find the headers

find_path(
  LIBNOISE_INCLUDE_DIR 
  NAMES noise.h
  PATH_SUFFIXES include include/noise
  HINTS ${LIBNOISE_DIR}
)

if(WIN32 AND NOT MINGW)
  # On windows we give CMake a little more guidance for 32 vs. 64 bit paths
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(NOISE_LIBRARY_PATH_SUFFIX lib/x64)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(NOISE_LIBRARY_PATH_SUFFIX lib/x86)
  endif()
endif()

# Find the library.
find_library(
  LIBNOISE_LIBRARIES
  NAMES libnoise noise libnoiseutils noiseutils
  PATHS
    PATH_SUFFIXES ${NOISE_LIBRARY_PATH_SUFFIX}
    HINTS ${LIBNOISE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Cytopia_ExternLibs/noise
)

if(LIBNOISE_INCLUDE_DIR AND LIBNOISE_LIBRARIES)
  # Create an imported library for libNoise
  add_library(LIBNOISE::LIBNOISE_LIBRARIES UNKNOWN IMPORTED)
  set_target_properties(
    LIBNOISE::LIBNOISE_LIBRARIES PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${LIBNOISE_INCLUDE_DIR}"
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION "${LIBNOISE_LIBRARIES}"
  )
endif()


# Ensure that everything was found correctly.
# This will also set LIBNOISE_FOUND to true/false.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LIBNOISE
  REQUIRED_VARS LIBNOISE_LIBRARIES LIBNOISE_INCLUDE_DIR
)

mark_as_advanced(
  LIBNOISE_INCLUDE_DIR
  LIBNOISE_LIBRARIES
)
