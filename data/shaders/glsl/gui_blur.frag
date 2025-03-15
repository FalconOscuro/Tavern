#version 450 core

// move out of file to keep constant?
#define BLUR_SIZE 7
#define BLUR_NUM_WEIGHTS ((BLUR_SIZE + 1) / 2)

uniform sampler2D tex;
uniform float weigths[BLUR_NUM_WEIGHTS];

uniform vec2 tex_coord_min;
uniform vec2 tex_coord_max;

in vec2 frag_tex_coord[BLUR_SIZE];

out vec4 frag_colour;

void main()
{
    vec4 colour = vec4(0.0);

    for (int i = 0; i < BLUR_SIZE; ++i) {
        vec2 in_region = step(tex_coord_min, frag_tex_coord[i]) * step(frag_tex_coord[i], tex_coord_max);
        colour += texture(tex, frag_tex_coord[i]) * in_region.x * in_region.y * weigths[abs(i - BLUR_NUM_WEIGHTS + 1)];
    }

    frag_colour = colour;
}
