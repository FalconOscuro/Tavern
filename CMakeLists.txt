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

add_library(Tavern-File STATIC)

target_include_directories(Tavern-File
    PUBLIC include/
)

target_sources(Tavern-File PRIVATE
    src/file/idir.cpp
    src/file/path.cpp

    src/file/physical_dir.cpp
    src/file/physical_file.cpp
    src/file/physical_mount.cpp

    src/file/tpk_dir.cpp
    src/file/tpk_file.cpp
    src/file/tpk_mount.cpp

    src/file/virtual_dir.cpp
    src/file/virtual_file.cpp
    src/file/virtual_mount.cpp

    src/file/tpk/tpk_package.cpp
)

# NOTE: Could drop boost log dependency
target_link_libraries(Tavern-File PRIVATE
    Boost::log
)

add_library(Cantrip STATIC)

target_sources(Cantrip PRIVATE
    src/cantrip/scanner/file_interface.cpp
    src/cantrip/scanner/scanner.cpp
    src/cantrip/scanner/token.cpp
    src/cantrip/scanner/file_pos.cpp

    src/cantrip/module/info.cpp

    src/cantrip/parser/parser-expr.cpp
    src/cantrip/parser/parser-stmt.cpp
    src/cantrip/parser/parser.cpp

    src/cantrip/ast/expression/identifier.cpp
    src/cantrip/ast/expression/literal.cpp

    src/cantrip/ast/statement/struct.cpp
    src/cantrip/ast/statement/type.cpp
    src/cantrip/ast/statement/var_declare.cpp
    
    src/cantrip/ast/visitor.cpp

    src/cantrip/analyzer/environment.cpp
    src/cantrip/analyzer/semantic.cpp

    src/cantrip/error/exception.cpp
    src/cantrip/error/semantic_error.cpp

    src/cantrip/runtime/object.cpp
    src/cantrip/runtime/tree_walk_interpreter.cpp
)

target_link_libraries(Cantrip PUBLIC Tavern-File)

add_executable(cantrip-bin src/cantrip-bin/main.cpp)
target_link_libraries(cantrip-bin PUBLIC
    Cantrip
    Boost::program_options
    Boost::log
)

message(STATUS "Configuring Tavern Library")
include(cmake/tavern.cmake)

message(STATUS "Configuring binaries")
add_executable(tavern-bin
    src/tavern-bin/main.cpp

    src/tavern-bin/panels/cantrip.cpp
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
