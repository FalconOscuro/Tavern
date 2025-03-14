#version 450 core

uniform float value;
uniform vec2 dimensions;

in vec2 frag_tex_coord;
in vec4 frag_colour;

out vec4 colour;

void main() {
    float t = value;
    vec3 c;
    float l;

    for (uint i = 0; i < 3; ++i)
    {
        vec2 p = frag_tex_coord;
        vec2 uv = p;

        p -= 0.5f;
        p.x *= dimensions.x / dimensions.y;
        
        float z = t + float(i) * .07;
        l = length(p);

        uv += p / l * (sin(z) + 1.f) * abs(sin(9. - (2 * z)));
        c[i] = .01 / length(mod(uv, 1.) - .5);
    }

    colour = vec4(c / l, frag_colour.a);
}
