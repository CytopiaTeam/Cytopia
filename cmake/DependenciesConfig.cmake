
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(LibNoise REQUIRED)

find_package(PNG)
if (NOT PNG_FOUND)
    find_package(PNG REQUIRED CONFIG)
endif ()

find_package(OpenAL CONFIG)
find_package(Vorbis)
cmake_dependent_option(ENABLE_AUDIO "use OPENAL" ON "OPENAL_FOUND AND VORBISFILE_FOUND" OFF)

find_package(Angelscript)
cmake_dependent_option(USE_ANGELSCRIPT "use Angelscript" ON "TARGET Angelscript::angelscript" OFF)

find_package(microprofile CONFIG)
option(USE_MICROPROFILE "use Microprofile" OFF)

include(FixSDLTargets)