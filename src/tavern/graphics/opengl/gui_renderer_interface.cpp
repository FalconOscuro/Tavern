#ifdef USE_OPENGL

#include "tavern/graphics/opengl/gui_render_interface.h"

#include <boost/log/trivial.hpp>

#include <glm/vec4.hpp>

#include <RmlUi/Core.h>

#include "tavern/graphics/mesh.h"
#include "tavern/graphics/texture.h"

#include "shaders/glsl_shaders.hpp"

#define MAX_NUM_GRADIENT_STOPS 16

namespace tavern::graphics::opengl {

// slightly hacky method for conversion, but overall simplifies code
template <typename T_OUT = glm::vec2, typename T_IN>
inline T_OUT rml_to_glm_vec2(const T_IN& v) {
    return T_OUT(v.x, v.y);
}

template <typename T_OUT = glm::vec3, typename T_IN>
inline T_OUT rml_to_glm_vec3(const T_IN& v) {
    return T_OUT(v.x, v.y, v.z);
}

inline glm::vec4 rml_colour_premult_to_vec4(const Rml::ColourbPremultiplied& c)
{
    glm::vec4 v;

    for (unsigned int i = 0; i < 4; ++i)
        v[i] = (1.f / 255.f) * static_cast<float>(c[i]);

    return v;
}

struct shader_data
{
    enum shader_type {
        INVALID = 0,
        VALID = 1,
        CREATION = VALID,
        GRADIENT = 1 << 1,
        REPEATING = 1 << 2,
        LINEAR_GRADIENT = GRADIENT | (0 << 3),
        RADIAL_GRADIENT = GRADIENT | (1 << 3),
        CONIC_GRADIENT  = GRADIENT | (2 << 3)
    } type;

    glm::vec2 v0;
    glm::vec2 v1;

    std::vector<glm::vec4> stop_colours;
    std::vector<float>     stop_positions;
}; /* end of struct shader_data */

gui_render_interface::~gui_render_interface() {
    shutdown();
}

bool gui_render_interface::init()
{
    // could just use a simple init flag, this is v. verbose
    // check if any shaders are still nullptr, if not initialization already completed, re-creation not needed
    if (m_colour_shader && m_texture_shader && m_gradient_shader && m_creation_shader && m_passthrough_shader && m_colour_matrix_shader && m_blend_mask_shader && m_blur_shader && m_drop_shadow_shader)
        return true;

    // shutdown first just incase of partial initialization, should be impossible
    shutdown();

    m_colour_shader        = new shader(glsl::gui::main_vert       , glsl::gui::colour_frag       );
    m_texture_shader       = new shader(glsl::gui::main_vert       , glsl::gui::texture_frag      );
    m_gradient_shader      = new shader(glsl::gui::main_vert       , glsl::gui::gradient_frag     );
    m_creation_shader      = new shader(glsl::gui::main_vert       , glsl::gui::creation_frag     );
    m_passthrough_shader   = new shader(glsl::gui::passthrough_vert, glsl::gui::passthrough_frag  );
    m_colour_matrix_shader = new shader(glsl::gui::passthrough_vert, glsl::gui::colour_matrix_frag);
    m_blend_mask_shader    = new shader(glsl::gui::passthrough_vert, glsl::gui::blend_mask_frag   );
    m_blur_shader          = new shader(glsl::gui::blur_vert       , glsl::gui::blur_frag         );
    m_drop_shadow_shader   = new shader(glsl::gui::passthrough_vert, glsl::gui::drop_shadow_frag  );
    return true;
}

void gui_render_interface::shutdown() {
    delete m_colour_shader;
    delete m_texture_shader;
    delete m_gradient_shader;
    delete m_creation_shader;
    delete m_passthrough_shader;
    delete m_colour_matrix_shader;
    delete m_blend_mask_shader;
    delete m_blur_shader;
    delete m_drop_shadow_shader;

    m_colour_shader        = nullptr;
    m_texture_shader       = nullptr;
    m_gradient_shader      = nullptr;
    m_creation_shader      = nullptr;
    m_passthrough_shader   = nullptr;
    m_colour_matrix_shader = nullptr;
    m_blend_mask_shader    = nullptr;
    m_blur_shader          = nullptr;
    m_drop_shadow_shader   = nullptr;
}

void gui_render_interface::begin_frame()
{
    glViewport(0, 0, m_screen_size.x, m_screen_size.y);

    glClearStencil(0);
    glClearColor(0, 0, 0, 0);

    glActiveTexture(GL_TEXTURE0);

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, GLuint(-1));
    glStencilMask(GLuint(-1));
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);
}

void gui_render_interface::end_frame()
{
}

Rml::CompiledGeometryHandle gui_render_interface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
{
    std::vector<vertex> conv_vertices;
    conv_vertices.reserve(vertices.size());

    for (auto v = vertices.begin(); v != vertices.end(); ++v) {
        vertex conv_vertex;

        // most of vertex is un-used!
        // wasted space
        conv_vertex.position = glm::vec3(rml_to_glm_vec2(v->position), 0.f);
        conv_vertex.texture_coordinates = rml_to_glm_vec2(v->position);
        conv_vertex.colour = glm::vec4(v->colour.red, v->colour.green, v->colour.blue, v->colour.alpha);

        conv_vertices.push_back(conv_vertex);
    }

    std::vector<uint32_t> conv_indices;
    conv_indices.reserve(indices.size());

    for (auto i = indices.begin(); i != indices.end(); ++i)
        conv_indices.push_back(*i);

    return reinterpret_cast<Rml::CompiledGeometryHandle>(new mesh(conv_vertices, conv_indices));
}

void gui_render_interface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture)
{
    glDisable(GL_DEPTH_TEST);

    const mesh* m = reinterpret_cast<const mesh*>(geometry);
    const texture2d* t = reinterpret_cast<const texture2d*>(texture);

    //BOOST_LOG_TRIVIAL(trace) << "Drawing geometry at: x(" << translation.x << "), y(" << translation.y << ")";

    m_texture_shader->use();
    m_texture_shader->set_vec2("screen_pos", rml_to_glm_vec2(translation));
    m_texture_shader->set_int("tex", 0);
    m_texture_shader->set_mat4x4("transform", m_projection);

    t->use();
    m->draw();
    t->disable();
    m_texture_shader->disable();
}

void gui_render_interface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
{
    delete reinterpret_cast<mesh*>(geometry);
}

Rml::TextureHandle gui_render_interface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
    glm::ivec3 dimensions;
    auto img_data = load_image_data(source.c_str(), dimensions);

    texture_dimensions.x = dimensions.x;
    texture_dimensions.y = dimensions.y;

    if (!img_data) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load image: " << source;
        return 0;
    }

    BOOST_LOG_TRIVIAL(trace) << "Loaded image: " << source;

    return reinterpret_cast<Rml::TextureHandle>(new texture2d(img_data.get(), dimensions));
}

Rml::TextureHandle gui_render_interface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
{
    const glm::ivec3 dimensions(rml_to_glm_vec2<glm::ivec2>(source_dimensions), 4);

    return (Rml::TextureHandle)new texture2d(source.data(), dimensions);
}

void gui_render_interface::ReleaseTexture(Rml::TextureHandle texture) {
    delete ((texture2d*)texture);
}

void gui_render_interface::EnableScissorRegion(bool enable)
{
    if (enable)
        glEnable(GL_SCISSOR_TEST);

    else
        glDisable(GL_SCISSOR_TEST);
}

void gui_render_interface::SetScissorRegion(Rml::Rectanglei region)
{
    m_scissor_rect_pos = glm::clamp(rml_to_glm_vec2<glm::ivec2>(region.BottomLeft()), glm::ivec2(0), m_screen_size);
    m_scissor_rect_size = rml_to_glm_vec2<glm::ivec2>(region.Size());

    EnableScissorRegion(true);
}

void gui_render_interface::EnableClipMask(bool enable) {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to enable clip mask: " << (enable ? "true" : "false");
}

void gui_render_interface::RenderToClipMask(Rml::ClipMaskOperation mask_operation, Rml::CompiledShaderHandle geometry, Rml::Vector2f translation) {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to render to clip mask";
}

void gui_render_interface::SetTransform(const Rml::Matrix4f* transform) {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to set transform";
}

Rml::LayerHandle gui_render_interface::PushLayer() {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to PushLayer";
    return 0;
}

void gui_render_interface::CompositeLayers(Rml::LayerHandle source, Rml::LayerHandle destination, Rml::BlendMode blend_mode, Rml::Span<const Rml::CompiledFilterHandle> filters) {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to CompositeLayers";
}

void gui_render_interface::PopLayer() {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to PopLayer";
}

Rml::TextureHandle gui_render_interface::SaveLayerAsTexture() {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to SaveLayerAsTexture";
    return 0;
}

Rml::CompiledFilterHandle gui_render_interface::SaveLayerAsMaskImage() {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to SaveLayerAsMaskImage";
    return 0;
}

Rml::CompiledFilterHandle gui_render_interface::CompileFilter(const Rml::String& name, const Rml::Dictionary& parameters) {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to CompileFilter";
    return 0;
}

void gui_render_interface::ReleaseFilter(Rml::CompiledFilterHandle filter) {
    BOOST_LOG_TRIVIAL(trace) << "Recieved call to ReleaseFilter";
}

Rml::CompiledShaderHandle gui_render_interface::CompileShader(const Rml::String& name, const Rml::Dictionary& parameters)
{
    shader_data sd = {};

    BOOST_LOG_TRIVIAL(trace) << "Compiling gui shader: " << name;

    // WARNING: Stop colours not handled
    if (name == "linear-gradient")
    {
        sd.type = shader_data::LINEAR_GRADIENT;
        sd.v0 = rml_to_glm_vec2(Rml::Get(parameters, "p0", Rml::Vector2f(0.f)));
        sd.v1 = rml_to_glm_vec2(Rml::Get(parameters, "p1", Rml::Vector2f(0.f)));
    }

    else if (name == "radial-gradient")
    {
        sd.type = shader_data::RADIAL_GRADIENT;
        sd.v0 = rml_to_glm_vec2(Rml::Get(parameters, "center", Rml::Vector2f(0.f)));
        sd.v1 = glm::vec2(1.f) / rml_to_glm_vec2(Rml::Get(parameters, "radius", Rml::Vector2f(1.f)));
    }

    else if (name == "conic-gradient")
    {
        sd.type = shader_data::CONIC_GRADIENT;
        sd.v0 = rml_to_glm_vec2(Rml::Get(parameters, "center", Rml::Vector2f(0.f)));

        const float angle = Rml::Get(parameters, "angle", 0.f);
        sd.v1 = glm::vec2(glm::cos(angle), glm::sin(angle));
    }

    // apply repeating flag & stop colours to any gradient shaders
    if (sd.type & shader_data::GRADIENT)
    {
        sd.type = static_cast<shader_data::shader_type>(sd.type | (Rml::Get(parameters, "repeating", false) ? shader_data::REPEATING : shader_data::VALID));

        auto found = parameters.find("color_stop_list");
        if (found == parameters.end() || found->second.GetType() != Rml::Variant::COLORSTOPLIST)
        {
            BOOST_LOG_TRIVIAL(error) << "Could not find colour stop list for gui gradient shader!";
            return 0;
        }

        const Rml::ColorStopList& colour_stop_list = found->second.GetReference<Rml::ColorStopList>();
        const size_t num_stops = colour_stop_list.size() < MAX_NUM_GRADIENT_STOPS ? colour_stop_list.size() : MAX_NUM_GRADIENT_STOPS;

        sd.stop_colours.resize(num_stops);
        sd.stop_positions.resize(num_stops);

        for (size_t i = 0; i < num_stops; ++i)
        {
            const Rml::ColorStop& stop = colour_stop_list[i];

            if (stop.position.unit != Rml::Unit::NUMBER)
            {
                BOOST_LOG_TRIVIAL(error) << "Parsing gui gradient shader failed, stop colour position was not a number!";
                return 0;
            }

            sd.stop_colours[i] = rml_colour_premult_to_vec4(stop.color);
            sd.stop_positions[i] = stop.position.number;
        }
    }

    else if (name == "shader" && Rml::Get(parameters, "value", Rml::String()) == "creation")
    {
        sd.type = shader_data::CREATION;
        sd.v0 = rml_to_glm_vec2(Rml::Get(parameters, "dimensions", Rml::Vector2f(0.f)));
    }

    if (sd.type != shader_data::INVALID)
        return reinterpret_cast<Rml::CompiledShaderHandle>(new shader_data(sd));

    BOOST_LOG_TRIVIAL(error) << "Unsupported shader type \"" << name << "\" for gui renderer";
    return 0;
}

void gui_render_interface::RenderShader(Rml::CompiledShaderHandle shader_handle, Rml::CompiledGeometryHandle geometry_handle, Rml::Vector2f translation, Rml::TextureHandle texture_handle)
{
    glDisable(GL_DEPTH_TEST);
    const shader_data* sd = reinterpret_cast<const shader_data*>(shader_handle);
    const mesh* m = reinterpret_cast<const mesh*>(geometry_handle);

    // unused
    const texture2d* t = reinterpret_cast<const texture2d*>(texture_handle);
    (void)t;

    const glm::vec2 pos = rml_to_glm_vec2(translation);

    if (!(sd->type & shader_data::VALID)) {
        BOOST_LOG_TRIVIAL(error) << "Invalid render shader passed to gui renderer: " << sd->type;
        return;
    }

    else if (sd->type & shader_data::GRADIENT)
    {
        if (sd->stop_colours.size() != sd->stop_positions.size()) {
            BOOST_LOG_TRIVIAL(error) << "Size mismatch between stop_colours and stop_positions in gui gradient shader";
            return;
        }
        size_t num_stops = sd->stop_colours.size();

        // set shader params
        m_gradient_shader->use();
        m_gradient_shader->set_int("function", (sd->type >> 1));
        m_gradient_shader->set_vec2("v0", sd->v0);
        m_gradient_shader->set_vec2("v1", sd->v1);

        m_gradient_shader->set_int("num_stops", num_stops);
        m_gradient_shader->set_vec4_arr("stop_colours", sd->stop_colours.data(), num_stops);
        m_gradient_shader->set_float_arr("stop_positions", sd->stop_positions.data(), num_stops);

        m_gradient_shader->set_vec2("translate", pos);

        // draw
        m->draw();
    }

    else {
        BOOST_LOG_TRIVIAL(warning) << "Unsupported gui shader type: " << sd->type;
    }
}

void gui_render_interface::ReleaseShader(Rml::CompiledShaderHandle shader_handle) {
    delete reinterpret_cast<shader_data*>(shader_handle);
}

void gui_render_interface::resize(const glm::ivec2& size) {
    m_screen_size = size;
    m_projection = glm::ortho(0, size.x, size.y, 0, -10000, 10000);
}

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
