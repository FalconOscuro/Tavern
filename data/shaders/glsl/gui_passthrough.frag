#version 450 core

uniform sampler2D tex;

in vec2 tex_coord;

out vec4 colour;

void main() {
    colour = texture(tex, tex_coord);
}
