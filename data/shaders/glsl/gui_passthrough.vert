#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord;

layout (location = 0) out vec2 frag_tex_coord;

void main() {
    frag_tex_coord = tex_coord;
    gl_Position = vec4(position, 0.0, 1.0);
}
