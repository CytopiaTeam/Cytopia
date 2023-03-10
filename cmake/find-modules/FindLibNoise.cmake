
find_package(libnoise QUIET CONFIG)

if (TARGET libnoise::libnoise)
    set(LibNoise_FOUND TRUE)
    return()
endif ()

include(FindPackageHandleStandardArgs)

find_path(LibNoise_INCLUDE_DIR noise.h
        PATH_SUFFIXES include include/noise include/libnoise
        HINTS ${LIBNOISE_DIR} $ENV{LIBNOISE_DIR}
        )
find_library(LibNoise_LIBRARY
        NAMES noise noiseutils noise-static noiseutils-static
        PATH_SUFFIXES lib/x64 lib/x86
        HINTS ${LIBNOISE_DIR} $ENV{LIBNOISE_DIR}
        )

find_package_handle_standard_args(LibNoise DEFAULT_MSG LibNoise_INCLUDE_DIR LibNoise_LIBRARY)

if (LibNoise_FOUND)
    add_library(libnoise::libnoise INTERFACE IMPORTED)
    set_target_properties(libnoise::libnoise PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LibNoise_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LibNoise_LIBRARY}"
            )
endif ()

mark_as_advanced(LibNoise_INCLUDE_DIR LibNoise_LIBRARY)

