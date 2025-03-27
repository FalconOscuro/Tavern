# imgui
add_library(imgui STATIC)

target_include_directories(imgui PUBLIC
    imgui
    imgui/backends
)

target_sources(imgui PUBLIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    # Sdl2 + opengl specific files TODO: depend on USE_OPENGL
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui
    PUBLIC SDL2::SDL2
    PUBLIC OpenGL::GL
)

# stb_img
add_library(stb_img STATIC)

set_target_properties(stb_img PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(stb_img PUBLIC STB_IMAGE_IMPLEMENTATION)

target_sources(stb_img PUBLIC
    stb/stb_image.h
)
