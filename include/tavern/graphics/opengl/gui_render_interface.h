#ifdef USE_OPENGL
#ifndef OPENGL_GUI_RENDER_INTERFACE_H
#define OPENGL_GUI_RENDER_INTERFACE_H

#include <glm/vec2.hpp>

#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Types.h>

#include "shader.h"

namespace tavern::graphics::opengl {

class gui_render_interface final : public Rml::RenderInterface
{
public:

    gui_render_interface() = default;
    ~gui_render_interface();

    bool init();
    void shutdown();

    void begin_frame();
    void end_frame();

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
    void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;
    void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture) override;

    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;

    void EnableClipMask(bool enable) override;
    void RenderToClipMask(Rml::ClipMaskOperation mask_operation, Rml::CompiledShaderHandle geometry, Rml::Vector2f translation) override;

    void SetTransform(const Rml::Matrix4f* transform) override;

    Rml::LayerHandle PushLayer() override;
    void CompositeLayers(Rml::LayerHandle source, Rml::LayerHandle destination, Rml::BlendMode blend_mode, Rml::Span<const Rml::CompiledFilterHandle> filters) override;
    void PopLayer() override;

    Rml::TextureHandle SaveLayerAsTexture() override;
    Rml::CompiledFilterHandle SaveLayerAsMaskImage() override;

    Rml::CompiledFilterHandle CompileFilter(const Rml::String& name, const Rml::Dictionary& parameters) override;
    void ReleaseFilter(Rml::CompiledFilterHandle filter) override;

    Rml::CompiledShaderHandle CompileShader(const Rml::String& name, const Rml::Dictionary& parameters) override;
    void RenderShader(Rml::CompiledShaderHandle shader_handle, Rml::CompiledGeometryHandle geometry_handle, Rml::Vector2f translation, Rml::TextureHandle texture_handle) override;
    void ReleaseShader(Rml::CompiledShaderHandle shader_handle) override;

    void resize(const glm::ivec2& size);

private:

    glm::ivec2 m_screen_size;
    glm::mat4 m_projection;

    glm::ivec2 m_scissor_rect_pos;
    glm::ivec2 m_scissor_rect_size;

    shader* m_colour_shader        = nullptr;
    shader* m_texture_shader       = nullptr;
    shader* m_gradient_shader      = nullptr;
    shader* m_creation_shader      = nullptr;
    shader* m_passthrough_shader   = nullptr;
    shader* m_colour_matrix_shader = nullptr;
    shader* m_blend_mask_shader    = nullptr;
    shader* m_blur_shader          = nullptr;
    shader* m_drop_shadow_shader   = nullptr;

}; /* end of class gui_render_interface final : public Rml::RenderInterface */

} /* end of namespace tavern::graphics::opengl */

#endif /* end of define guard OPENGL_GUI_RENDER_INTERFACE_H */
#endif // Use opengl
