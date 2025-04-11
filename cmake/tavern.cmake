add_library(Tavern STATIC)

target_include_directories(Tavern
    PUBLIC include/
)

target_sources(Tavern PRIVATE
    src/tavern/core/engine.cpp
    src/tavern/core/file_system.cpp
    src/tavern/core/input.cpp
    src/tavern/core/scene.cpp
    src/tavern/core/scene_load.cpp
    src/tavern/core/scene_save.cpp
    src/tavern/core/window.cpp

    src/tavern/resource/mesh_manager.cpp
    src/tavern/resource/shader_manager.cpp
    src/tavern/resource/texture2d_manager.cpp
    src/tavern/resource/material_manager.cpp

    src/tavern/components/transform.cpp
    src/tavern/components/component_yaml_conversions.cpp

    src/tavern/graphics/generic/texture.cpp
    src/tavern/graphics/generic/renderer.cpp

    src/tavern/assets/pipeline.cpp
    src/tavern/assets/model_convert.cpp
)

target_link_libraries(Tavern
    PUBLIC ecs
    PUBLIC Tavern-File
    PUBLIC glm::glm
    PUBLIC SDL2::SDL2
    PUBLIC ryml::ryml
    PUBLIC imgui
    PUBLIC stb_img
    PRIVATE assimp
    PRIVATE Boost::log
)

if(RENDERER STREQUAL "gl")
    target_link_libraries(Tavern PUBLIC
        GLEW::glew
        OpenGL::GL
        OpenGL::GLU
    )

    target_sources(Tavern PUBLIC
        src/tavern/graphics/opengl/renderer.cpp
        src/tavern/graphics/opengl/shader.cpp
        src/tavern/graphics/opengl/texture_atlas.cpp
        src/tavern/graphics/opengl/mesh.cpp
        src/tavern/graphics/opengl/texture2d.cpp
    )

    target_compile_definitions(Tavern
        PUBLIC USE_OPENGL RENDERER_NS=opengl
    )

    target_include_directories(Tavern PUBLIC ${PROJECT_BINARY_DIR}/configure/)

    # could be simplified w/ function
    file(READ data/shaders/glsl/pbr.vert PBR_VERT)
    file(READ data/shaders/glsl/pbr.frag PBR_FRAG)

    file(READ data/shaders/glsl/gui_main.vert GUI_MAIN_VERT)

    file(READ data/shaders/glsl/gui_texture.frag GUI_TEXTURE_FRAG)
    file(READ data/shaders/glsl/gui_colour.frag GUI_COLOUR_FRAG)
    file(READ data/shaders/glsl/gui_gradient.frag GUI_GRADIENT_FRAG)
    file(READ data/shaders/glsl/gui_creation.frag GUI_CREATION_FRAG)
    
    file(READ data/shaders/glsl/gui_passthrough.vert GUI_PASSTHROUGH_VERT)
    file(READ data/shaders/glsl/gui_passthrough.frag GUI_PASSTHROUGH_FRAG)

    file(READ data/shaders/glsl/gui_colour_matrix.frag GUI_COLOUR_MATRIX_FRAG)
    file(READ data/shaders/glsl/gui_blend_mask.frag GUI_BLEND_MASK_FRAG)

    file(READ data/shaders/glsl/gui_blur.vert GUI_BLUR_VERT)
    file(READ data/shaders/glsl/gui_blur.frag GUI_BLUR_FRAG)

    file(READ data/shaders/glsl/gui_drop_shadow.frag GUI_DROP_SHADOW_FRAG)

    configure_file(data/shaders/glsl/glsl_shaders.hpp.in configure/shaders/glsl_shaders.hpp @ONLY)

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        data/shaders/glsl/pbr.vert
        data/shaders/glsl/pbr.frag

        data/shaders/glsl/gui_main.vert

        data/shaders/glsl/gui_texture.frag
        data/shaders/glsl/gui_colour.frag
        data/shaders/glsl/gui_gradient.frag
        data/shaders/glsl/gui_creation.frag

        data/shaders/glsl/gui_passthrough.vert
        data/shaders/glsl/gui_passthrough.frag

        data/shaders/glsl/gui_colour_matrix.frag
        data/shaders/glsl/gui_blend_mask.frag

        data/shaders/glsl/gui_blur.vert
        data/shaders/glsl/gui_blur.frag

        data/shaders/glsl/gui_drop_shadow.frag

        data/shaders/glsl/glsl_shaders.hpp.in
    )
endif()
