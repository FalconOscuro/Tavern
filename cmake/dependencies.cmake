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
