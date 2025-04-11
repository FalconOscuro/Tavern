#==========================================
# imgui
#==========================================
add_library(imgui STATIC)

target_include_directories(imgui PUBLIC
    imgui
    imgui/backends
)

target_sources(imgui PRIVATE
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_sdl2.cpp
)

target_link_libraries(imgui PUBLIC SDL2::SDL2)

if(RENDERER STREQUAL "gl")
    target_link_libraries(imgui PUBLIC OpenGL::GL)

    target_sources(imgui PUBLIC imgui/backends/imgui_impl_opengl3.cpp)
endif()

#==========================================
# stb_img
#==========================================
add_library(stb_img STATIC)

target_include_directories(stb_img PUBLIC
    stb
)

target_sources(stb_img PRIVATE
    src/stb_image_impl.cpp
)
