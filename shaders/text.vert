#version 450 core

// keep consistant with frame size over all calls for singular font, mutating with passed args
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coords;

layout (location = 0) uniform mat4 transform;

uniform mat4 projection;

layout (location = 0) out vec2 tex_coords_out;

void main()
{
    tex_coords_out = tex_coords;

    gl_Position = projection * transform * vec4(pos, 0, 1.0);
}
