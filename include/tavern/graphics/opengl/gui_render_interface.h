#include "RmlUi/Core/Types.h"
#ifdef USE_OPENGL
#ifndef OPENGL_GUI_RENDER_INTERFACE_H
#define OPENGL_GUI_RENDER_INTERFACE_H

#include <glm/vec2.hpp>

#include <RmlUi/Core/RenderInterface.h>

namespace tavern::graphics::opengl {

class gui_render_interface final : public Rml::RenderInterface
{
public:

    gui_render_interface() {}
    ~gui_render_interface() = default;

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
    void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;
    void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture) override;

    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;

    Rml::CompiledShaderHandle CompileShader(const Rml::String& name, const Rml::Dictionary& parameters) override { return 0; }
    void RenderShader(Rml::CompiledShaderHandle shader_handle, Rml::CompiledGeometryHandle geometry_handle, Rml::Vector2f translation, Rml::TextureHandle texture_handle) override {};
    void ReleaseShader(Rml::CompiledShaderHandle shader_handle) override {};

    void resize(const glm::ivec2& size);

}; /* end of class gui_render_interface final : public Rml::RenderInterface */

} /* end of namespace tavern::graphics::opengl */

#endif /* end of define guard OPENGL_GUI_RENDER_INTERFACE_H */
#endif // Use opengl
