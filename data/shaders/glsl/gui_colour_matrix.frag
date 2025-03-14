#version 450 core

uniform sampler2D tex;
uniform mat4 colour_matrix;

in vec2 frag_tex_coord;

out vec4 colour;

void main() {
    vec4 tex_colour = texture(tex, frag_tex_coord);
    vec3 transformed_colour = vec3(colour_matrix * tex_colour);

    colour = vec4(transformed_colour, tex_colour.a);
}
