cmake_minimum_required(VERSION 3.30)

project(Tavern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)

set(RENDERER "gl" CACHE STRING "Renderering library used (default: OpenGL).")
set_property(CACHE RENDERER PROPERTY STRINGS gl vk)

include(FetchContent)

FetchContent_Declare(
    ecs
    GIT_REPOSITORY git@github.com:FalconOscuro/Tavern-ECS.git
    GIT_TAG origin/main
)

FetchContent_Declare(
    ttf2bmp
    GIT_REPOSITORY git@github.com:FalconOscuro/ttf2bmp.git
    GIT_TAG v1.0
    GIT_SHALLOW FALSE
)

FetchContent_Declare(
    ryml
    GIT_REPOSITORY git@github.com:biojppm/rapidyaml.git
    GIT_TAG v0.7.2
    GIT_SHALLOW FALSE
)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY git@github.com:assimp/assimp.git
    GIT_TAG v5.4.3
)

# assimp build options
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)

set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)

#set(ASSIMP_BUILD_ASSBIN_IMPORTER ON)
#set(ASSIMP_BUILD_COLLADA_IMPORTER ON)
#set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(ASSIMP_BUILD_GLTF_IMPORTER ON)

FetchContent_MakeAvailable(ecs ttf2bmp ryml assimp)

find_package(Boost REQUIRED COMPONENTS log program_options)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(extern)

add_library(Tavern STATIC)

target_include_directories(Tavern
    PUBLIC include/
    PUBLIC extern/
    PUBLIC extern/imgui
)

target_sources(Tavern PUBLIC
    src/tavern/tavern.cpp

    src/tavern/core/window.cpp
    src/tavern/core/input.cpp

    src/tavern/resource/mesh_manager.cpp
    src/tavern/resource/shader_manager.cpp
    src/tavern/resource/texture2d_manager.cpp

    src/tavern/systems/scene.cpp

    src/tavern/components/camera.cpp
    src/tavern/components/drawable3d.cpp
    src/tavern/components/transform3d.cpp

    src/tavern/file/physical_mount.cpp
    src/tavern/file/physical_file.cpp
    src/tavern/file/tpk_mount.cpp
    src/tavern/file/tpk_file.cpp

    src/tavern/gui/label.cpp
)

target_link_libraries(Tavern
    PUBLIC ecs
    PUBLIC ttf2bmp
    PUBLIC SDL2::SDL2
    PUBLIC Boost::log
    PUBLIC ryml::ryml
    PUBLIC assimp
    PUBLIC glm::glm
    PUBLIC imgui
)

if(${RENDERER} STREQUAL "gl")
    message(STATUS "Using OpenGL")

    target_link_libraries(Tavern PUBLIC
        GLEW::glew
        OpenGL::GL
        OpenGL::GLU
    )

    target_sources(Tavern PUBLIC
        src/tavern/graphics/opengl/font.cpp
        src/tavern/graphics/opengl/renderer.cpp
        src/tavern/graphics/opengl/shader.cpp
        src/tavern/graphics/opengl/texture_atlas.cpp
        src/tavern/graphics/opengl/mesh.cpp
        src/tavern/graphics/opengl/texture2d.cpp
    )

    target_compile_definitions(Tavern
        PUBLIC USE_OPENGL RENDERER_NS=opengl
    )

elseif(${RENDERER} STREQUAL "vk")
    message(FATAL_ERROR "Vulkan is not currently supported")
else()
    message(FATAL_ERROR "Unrecognized renderer")
endif()

add_executable(tavern-bin
    src/tavern-bin/main.cpp
)

target_link_libraries(tavern-bin PUBLIC
    Tavern
    Boost::program_options
    Boost::log
)

add_custom_command(
    TARGET tavern-bin PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/shaders"
        "${PROJECT_BINARY_DIR}/shaders"
)
