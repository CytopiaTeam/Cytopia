set(IMGUI_SOURCES
        backends/imgui_impl_sdl.cpp
        backends/imgui_impl_sdl.h
        backends/imgui_impl_sdlrenderer.cpp
        backends/imgui_impl_sdlrenderer.h
        imconfig.h
        imgui.cpp
        imgui.h
        imgui_draw.cpp
        imgui_internal.h
        imgui_tables.cpp
        imgui_widgets.cpp
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h
        )

add_library(IMGUI STATIC ${IMGUI_SOURCES})

target_link_libraries(IMGUI ${TESTS_PROJECT_NAME} PUBLIC SDL2::SDL2)
target_include_directories(IMGUI PUBLIC . backends)