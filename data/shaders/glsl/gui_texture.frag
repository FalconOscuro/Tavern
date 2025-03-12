#version 450 core

in vec2 frag_tex_coord;
in vec4 frag_colour;

uniform sampler2D tex;

out vec4 colour;

void main() {
    vec4 tex_colour = texture(tex, frag_tex_coord);
    colour = frag_colour * tex_colour;
}
