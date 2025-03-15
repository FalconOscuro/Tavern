#version 450 core

uniform sampler2D tex;
uniform sampler2D tex_mask;

in vec2 frag_tex_coord;

out vec4 colour;

void main() {
    vec4 tex_colour = texture(tex, frag_tex_coord);
    float mask_alpha = texture(tex_mask, frag_tex_coord).a;

    colour = tex_colour * mask_alpha;
}
