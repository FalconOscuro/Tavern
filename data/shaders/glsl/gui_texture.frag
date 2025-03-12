#version core 450

layout (location = 0) in vec2 tex_coord;
layout (location = 1) in vec4 colour;

uniform sampler2D tex;

out vec4 out_colour;

void main() {
    vec4 tex_colour = texture(tex, tex_coord);
    out_colour = colour * tex_colour;
}
