#version 450 core

// move out of file to keep constant?
#define BLUR_SIZE 7
#define BLUR_NUM_WEIGHTS ((BLUR_SIZE + 1) / 2)

uniform vec2 texel_offset;

layout (location = 0) in vec3 position;
layout (location = 4) in vec2 tex_coord;

out vec2 frag_tex_coord[BLUR_SIZE];

void main()
{
    for (uint i = 0; i < BLUR_SIZE; ++i)
        frag_tex_coord[i] = tex_coord - float(i - BLUR_NUM_WEIGHTS + 1) * texel_offset;

    gl_Position = vec4(position, 1.0);
}
