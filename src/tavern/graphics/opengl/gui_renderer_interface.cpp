#ifdef USE_OPENGL

#include "tavern/graphics/opengl/gui_render_interface.h"

#include "tavern/graphics/mesh.h"
#include "tavern/graphics/texture.h"

namespace tavern::graphics::opengl {

Rml::CompiledGeometryHandle gui_render_interface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
{
    std::vector<vertex> conv_vertices;
    conv_vertices.reserve(vertices.size());

    for (auto v = vertices.begin(); v != vertices.end(); ++v) {
        vertex conv_vertex;

        // normal tangent & bitangent un-used!!
        // wasted space
        conv_vertex.position = glm::vec3(v->position.x, v->position.y, 0.f);
        conv_vertex.texture_coordinates = glm::vec2(v->position.x, v->position.y);

        conv_vertices.push_back(conv_vertex);
    }

    std::vector<uint32_t> conv_indices;
    conv_indices.reserve(indices.size());

    for (auto i = indices.begin(); i != indices.end(); ++i)
        conv_indices.push_back(*i);

    return (Rml::CompiledGeometryHandle)new mesh(conv_vertices, conv_indices);
}

void gui_render_interface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture)
{
    mesh* m = (mesh*)geometry;
    texture2d* t = (texture2d*)texture;

    // need shader
    m->draw();
}

void gui_render_interface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
{
    delete ((mesh*)geometry);
}

Rml::TextureHandle gui_render_interface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
    glm::ivec3 dimensions;
    auto img_data = load_image_data(source.c_str(), dimensions);

    texture_dimensions.x = dimensions.x;
    texture_dimensions.y = dimensions.y;

    if (!img_data)
        return 0;

    return (Rml::TextureHandle)new texture2d(img_data.get(), dimensions);
}

Rml::TextureHandle gui_render_interface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
{
    const glm::ivec3 dimensions(source_dimensions.x, source_dimensions.y, 4);

    return (Rml::TextureHandle)new texture2d(source.data(), dimensions);
}

void gui_render_interface::ReleaseTexture(Rml::TextureHandle texture) {
    delete ((texture2d*)texture);
}

void gui_render_interface::EnableScissorRegion(bool enable) {
}

void gui_render_interface::SetScissorRegion(Rml::Rectanglei region) {
}

void gui_render_interface::resize(const glm::ivec2& size) {
}

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
