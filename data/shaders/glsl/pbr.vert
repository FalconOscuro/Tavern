#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bi_tangent;
layout (location = 4) in vec2 tex_coords;

// Uniform buffer object for camera projection and view matricies as only updated once per render loop
// set to std140 to maintain c++ packing
layout (std140, binding = 0) uniform camera_uniforms
{
    mat4 projection;
    mat4 view;
};

layout (location = 0) uniform mat4 model;

layout (location = 0) out vertex
{
    vec3 position;
    vec2 tex_coords;
    mat3 tangent_basis;
} vertex_out;

void main()
{
    vertex_out.position  = vec3(model * vec4(position, 1.0f));
    vertex_out.tex_coords = tex_coords;

    // Tangent space vectors for normal mapping
    vertex_out.tangent_basis = mat3(model) * mat3(tangent, bi_tangent, normal);

    // position in screen space
    gl_Position = projection * view * model * vec4(position, 1.0f);
}
