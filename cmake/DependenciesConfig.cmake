include(DependenciesFunctions)
find_package(Threads REQUIRED)

add_external_lib(
        SDL2
        sdl/2.26.0
        REQUIRED
        INTERFACE_NAME SDL::SDL
        PKG_CONFIG "sdl2 >= 2.0"
)
add_external_lib(
        SDL2_image
        sdl_image/2.0.5
        REQUIRED
        INTERFACE_NAME SDL::SDL_image
        PKG_CONFIG "SDL2_image"
)
add_external_lib(
        SDL2_ttf
        sdl_ttf/2.0.18
        REQUIRED
        INTERFACE_NAME SDL::TTF
        PKG_CONFIG "SDL2_ttf"
)

add_external_lib(
        ZLIB
        zlib/1.2.13
        REQUIRED
        PKG_CONFIG "zlib"
)
add_external_lib(
        LibNoise
        libnoise/1.0.0
        REQUIRED
        FIND_PACKAGE
)
add_external_lib(
        LIBPNG
        libpng/1.6.39
        REQUIRED
        PKG_CONFIG "libpng"
)

add_external_lib(
        OpenAL
        openal/1.22.2
        PKG_CONFIG "openal >= 1.18"
        FIND_PACKAGE_OPTIONS CONFIG
)

add_external_lib(
        VorbisFile
        vorbis/1.3.7
        PKG_CONFIG "vorbis >= 1.2, vorbisfile >= 1.2"
        FIND_PACKAGE
)

add_external_lib(
        Angelscript
        angelscript/2.35.1
        FIND_PACKAGE
        BY_DEFAULT_DISABLED
)
add_external_lib(
        Microprofile
        microprofile/3.1
        FIND_PACKAGE
        BY_DEFAULT_DISABLED
)

if (UNIX AND NOT APPLE)
        add_external_lib(
                libalsa
                libalsa/1.2.7.2
        )
endif ()