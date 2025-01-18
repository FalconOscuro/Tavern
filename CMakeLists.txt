cmake_minimum_required(VERSION 3.30)

project(Tavern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RENDERER "gl" CACHE STRING "Renderering library used (default: OpenGL).")
set_property(CACHE RENDERER PROPERTY STRINGS gl vk)

include(FetchContent)

FetchContent_Declare(
    ecs
    GIT_REPOSITORY git@github.com:FalconOscuro/Tavern-ECS.git
    GIT_TAG origin/main
    GIT_PROGRESS TRUE
)

FetchContent_Declare(
    ryml
    GIT_REPOSITORY git@github.com:biojppm/rapidyaml.git
    GIT_TAG v0.7.2
    GIT_SHALLOW FALSE
)

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY git@github.com:tinyobjloader/tinyobjloader.git
    GIT_TAG v2.0.0rc13
)

FetchContent_MakeAvailable(ecs ryml tinyobjloader)

find_package(Boost COMPONENTS log)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_library(Tavern STATIC)

target_include_directories(Tavern
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_sources(Tavern PUBLIC
    src/tavern/tavern.cpp
    src/tavern/core/window.cpp
    src/tavern/graphics/opengl/renderer.cpp
    src/tavern/graphics/opengl/mesh.cpp
    src/tavern/graphics/opengl/shader.cpp
    src/tavern/resource/mesh_manager.cpp
)

target_link_libraries(Tavern
    PUBLIC ecs
    PUBLIC SDL2::SDL2
    PUBLIC Boost::log
    PUBLIC ryml::ryml
    PUBLIC tinyobjloader
)

if(${RENDERER} STREQUAL "gl")
    message(STATUS "Using OpenGL")

    target_link_libraries(Tavern PUBLIC
        GLEW::glew
        OpenGL::GL
        OpenGL::GLU
    )

    target_compile_definitions(Tavern PUBLIC USE_OPENGL)

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
)
