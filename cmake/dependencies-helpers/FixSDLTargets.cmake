if (NOT TARGET SDL2_ttf::SDL2_ttf)
    message(STATUS "Adding SDL2_ttf::SDL2_ttf target")
    add_library(SDL2_ttf::SDL2_ttf ALIAS sdl_ttf::sdl_ttf)
endif ()