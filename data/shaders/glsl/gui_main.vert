#version 450 core

layout (location = 0) in vec3 position;
layout (location = 4) in vec2 tex_coord;
layout (location = 5) in vec4 colour;

uniform vec2 translate;

out vec2 frag_tex_coord;
out vec4 frag_colour;

void main() {
    frag_tex_coord = tex_coord;
    frag_colour = colour;
}
