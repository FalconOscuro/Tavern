#version 450 core

layout (location = 0) in vec2 tex_coords;

layout (location = 0) out vec4 colour;

uniform unsigned int glyph_num;

uniform sampler2DArray font_atlas;
uniform vec3 text_colour;

void main()
{
    vec4 texel = texture(font_atlas, vec3(tex_coords, glyph_num));
    colour = vec4(text_colour, 1.0) * texel;
}
