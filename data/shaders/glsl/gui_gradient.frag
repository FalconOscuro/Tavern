#version 450 core

#define REPEATING uint(1)

#define LINEAR uint(0 << 1)
#define RADIAL uint(1 << 1)
#define CONIC  uint(2 << 1)

#define MAX_NUM_STOPS 16

#define PI 3.13159265

in vec2 frag_tex_coord;
in vec4 frag_colour;

uniform uint function;
uniform vec2 v0; // linear: start point, radial: center                      , conic: center
uniform vec2 v1; // linear: end point  , radial: 2d curvature(inverse radius), conic: angled unit vector

uniform vec4 stop_colours[MAX_NUM_STOPS];
uniform float stop_positions[MAX_NUM_STOPS];
uniform uint num_stops;

out vec4 colour;

vec4 mix_stop_colours(float t) {
    vec4 c = stop_colours[0];

    for (uint i = 1; i < num_stops; ++i)
        c = mix(c, stop_colours[i], smoothstep(stop_positions[i-1], stop_positions[i], t));

    return c;
}

void main()
{
    float t = 0.f;

    if ((function & RADIAL) > 0)
    {
        vec2 V = frag_tex_coord - v0;
        t = length(v1 * V);
    }

    else if ((function & CONIC) > 0)
    {
        mat2 R = mat2(v1.x, -v1.y, v1.y, v1.x);
        vec2 V = R * (frag_tex_coord - v0);
        t = 0.5 + atan(-V.x, V.y) / (2.0 * PI);
    }

    else
    {
        float dist_squared = dot(v1, v1);
        vec2 V = frag_tex_coord - v0;
        t = dot(v1, V) / dist_squared;
    }

    if ((function & REPEATING) > 0)
    {
        float t0 = stop_positions[0];
        float t1 = stop_positions[num_stops - 1];

        t = t0 + mod(t - t0, t1 - t0);
    }

    colour = frag_colour * mix_stop_colours(t);
}
