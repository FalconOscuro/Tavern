#version 450 core

uniform sampler2D tex;
uniform vec2 tex_coord_min;
uniform vec2 tex_coord_max;
uniform vec4 shadow_colour;

in vec2 frag_tex_coord;

out vec4 frag_colour;

void main() {
    vec2 in_region = step(tex_coord_min, frag_tex_coord) * step(frag_tex_coord, tex_coord_max);
    frag_colour = texture(tex, frag_tex_coord).a * in_region.x * in_region.y * shadow_colour;
}
