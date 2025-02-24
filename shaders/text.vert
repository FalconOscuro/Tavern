#version 450 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coords_in;

uniform mat4 projection;

layout (location = 0) out vec2 tex_coords;


void main()
{
    tex_coords = tex_coords_in;
    gl_Position = projection * vec4(pos, 0.0, 1.0);
}
