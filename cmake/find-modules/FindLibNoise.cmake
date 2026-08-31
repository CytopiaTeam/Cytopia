
find_package(libnoise QUIET CONFIG)

if (TARGET libnoise::libnoise)
    set(LibNoise_FOUND TRUE)
    return()
endif ()

include(FindPackageHandleStandardArgs)

# Cytopia includes libnoise as <noise/noise.h>, so the include directory must
# be the parent of the noise/ folder (e.g. <prefix>/include), not the folder itself.
find_path(LibNoise_INCLUDE_DIR noise/noise.h
        PATH_SUFFIXES include
        HINTS ${LIBNOISE_DIR} $ENV{LIBNOISE_DIR}
        )

# Debian/Ubuntu install the headers in <prefix>/include/libnoise/ instead of
# <prefix>/include/noise/. Provide a compatibility symlink so that
# <noise/noise.h> keeps working with distro packages.
if (NOT LibNoise_INCLUDE_DIR)
    find_path(LibNoise_INCLUDE_DIR libnoise/noise.h
            PATH_SUFFIXES include
            HINTS ${LIBNOISE_DIR} $ENV{LIBNOISE_DIR}
            )
    if (LibNoise_INCLUDE_DIR)
        set(_noise_compat_dir "${CMAKE_BINARY_DIR}/noise-compat")
        file(MAKE_DIRECTORY "${_noise_compat_dir}")
        if (NOT EXISTS "${_noise_compat_dir}/noise")
            file(CREATE_LINK "${LibNoise_INCLUDE_DIR}/libnoise" "${_noise_compat_dir}/noise" SYMBOLIC)
        endif ()
        set(LibNoise_INCLUDE_DIR "${_noise_compat_dir}")
    endif ()
endif ()
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

