#version 450 core

layout (location = 0) in vec3 position;
layout (location = 4) in vec2 tex_coord;
layout (location = 5) in vec4 colour;

uniform vec2 screen_pos;
uniform mat4 transform;

out vec2 frag_tex_coord;
out vec4 frag_colour;

void main() {
    frag_tex_coord = tex_coord;
    frag_colour = colour;

    gl_Position = transform * vec4(position + vec3(screen_pos, 0.0f), 1.0f);
}
