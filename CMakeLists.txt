cmake_minimum_required(VERSION 3.30)

project(Tavern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)

set(RENDERER "gl" CACHE STRING "Renderering library used (default: OpenGL).")
set_property(CACHE RENDERER PROPERTY STRINGS gl vk)

if(RENDERER STREQUAL "gl")
    message(STATUS "Using renderer backend: OpenGL")
elseif(RENDERER STREQUAL "vk")
    message(FATAL_ERROR "Vulkan is not currently supported")
else()
    message(FATAL_ERROR "Unrecognized renderer backend: ${RENDERER}")
endif()

message(STATUS "Configuring dependencies")
include(cmake/dependencies.cmake)

add_subdirectory(extern)

message(STATUS "Configuring Tavern Library")
include(cmake/tavern.cmake)

message(STATUS "Configuring binaries")
add_executable(tavern-bin
    src/tavern-bin/main.cpp

    src/tavern-bin/panels/performance.cpp
    src/tavern-bin/panels/scene.cpp
    src/tavern-bin/panels/file_system.cpp
)

target_link_libraries(tavern-bin PUBLIC
    Tavern
    Boost::program_options
    Boost::log
)

add_executable(asset_pipeline
    src/tavern-bin/asset_pipeline.cpp
)

target_link_libraries(asset_pipeline PUBLIC
    Tavern
    Boost::program_options
    Boost::log
)
