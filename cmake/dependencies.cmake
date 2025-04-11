#==========================================
# Fetchcontent dependencies
#==========================================
include(FetchContent)

#============================
# ryml for yaml files
#============================
FetchContent_Declare(
    ryml
    GIT_REPOSITORY git@github.com:biojppm/rapidyaml.git
    GIT_TAG v0.8.0
    GIT_SHALLOW FALSE
)

#============================
# assimp for 3D file formats
#============================
FetchContent_Declare(
    assimp
    GIT_REPOSITORY git@github.com:assimp/assimp.git
    GIT_TAG v5.4.3
)

# assimp build options
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)

set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)

FetchContent_MakeAvailable(ryml assimp)

#==========================================
# local dependencies
#==========================================
find_package(Boost REQUIRED COMPONENTS log program_options)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)

if (RENDERER STREQUAL "gl")
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
endif()
