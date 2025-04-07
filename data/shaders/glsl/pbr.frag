#version 450 core

layout (location = 0) in vertex
{
    vec3 position;
    vec2 tex_coords;
    mat3 tangent_basis;
} vertex_in;

struct material
{
    bool use_albedo_tex;
    bool use_metallic_tex;
    bool use_roughness_tex;
    bool use_normal_tex;
    bool use_ambient_occlusion_tex;
    bool use_emissive_tex;

    vec3 albedo;
    float metallic;
    float roughness;
    float ambient_occlusion;
    vec3 emissive;

    sampler2D albedo_tex;
    sampler2D metallic_tex;
    sampler2D roughness_tex;
    sampler2D normal_tex;
    sampler2D ambient_occlusion_tex;
    sampler2D emissive_tex;
};

uniform material mat;

layout (location = 0) out vec4 frag_colour;

vec3 calculate_normal(vec3 tangent_normal) {
    vec3 norm = normalize(2.0f * tangent_normal - 1.0f);
    return normalize(vertex_in.tangent_basis * norm);
}

void main()
{
    vec3 albedo = mat.albedo;
    if (mat.use_albedo_tex)
        albedo = texture(mat.albedo_tex, vertex_in.tex_coords).rgb;

    float metallic = mat.metallic;
    if (mat.use_metallic_tex)
        metallic = texture(mat.metallic_tex, vertex_in.tex_coords).r;

    float roughness = mat.roughness;
    if (mat.use_roughness_tex)
        roughness = texture(mat.roughness_tex, vertex_in.tex_coords).r;

    vec3 normal = vec3(vertex_in.tangent_basis);
    if (mat.use_normal_tex)
        normal = calculate_normal(texture(mat.normal_tex, vertex_in.tex_coords).rgb);

    float ambient_occlusion = mat.ambient_occlusion;
    if (mat.use_ambient_occlusion_tex)
        ambient_occlusion = texture(mat.ambient_occlusion_tex, vertex_in.tex_coords).r;

    vec3 emissive = mat.emissive;
    if (mat.use_emissive_tex)
        emissive = texture(mat.emissive_tex, vertex_in.tex_coords).rgb;

    vec3 colour = albedo;
    colour *= ambient_occlusion;
    colour += emissive;

    frag_colour = vec4(colour, 1.0f);
}
