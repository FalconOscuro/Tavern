#ifdef USE_OPENGL

#include "tavern/graphics/opengl/renderer.hpp"

#include <cassert>

#include <boost/log/trivial.hpp>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "tavern/core/scene.h"
#include "tavern/core/resource_manager.h"

#include "tavern/components/render_mesh.h"
#include "tavern/components/transform.h"
#include "tavern/components/camera.h"

#include "tavern/platform/sdl.h"

#include "shaders/glsl_shaders.hpp"

namespace tavern::graphics::opengl {

struct camera_ub {
    glm::mat4 projection;
    glm::mat4 view;
}; /* end of struct camera_ub */

bool renderer::pre_window_init()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // NDEBUG

    return true;
}

void renderer::shutdown_renderer() {

    if (!m_glcontext)
        return;

    // shutdown imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // release ownership of default shader
    m_default_shader.reset();
    glDeleteBuffers(1, &m_camera_ub);

    SDL_GL_DeleteContext(m_glcontext);
    BOOST_LOG_TRIVIAL(trace) << "Shutdown OpenGL";
    m_glcontext = NULL;
}

bool renderer::init(void* window, const glm::ivec2 size)
{
    m_glcontext = SDL_GL_CreateContext((SDL_Window*)window);

    if (!m_glcontext) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create OpenGL context:\n"
            << SDL_GetError();
        return false;
    }

    // init glew
    glewExperimental = true;
    GLenum glew_error = glewInit();

    if (glew_error != GLEW_OK) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize GLEW: " << glewGetErrorString(glew_error);
        return false;
    }

    BOOST_LOG_TRIVIAL(info) << "GPU: " << get_gpu_name();
    BOOST_LOG_TRIVIAL(info) << get_renderer_info();

    BOOST_LOG_TRIVIAL(trace) << "Initialized OpenGL";

    // cull rear side of faces
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glFrontFace(GL_CCW);

    set_viewport_size(size);

    // load default shader
    // Check to ensure successful completion?
    m_default_shader = std::make_unique<shader>(glsl::pbr::vert, glsl::pbr::frag);

    // create uniform buffer for camera data
    m_camera_ub = create_uniform_buffer<camera_ub>();

    // init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)window, m_glcontext);
    ImGui_ImplOpenGL3_Init();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    BOOST_LOG_TRIVIAL(trace) << "Initialized imgui";

    return true;
}

void renderer::clear()
{
    glClearColor(0.f, 0.f, .2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer::render()
{
    render_geometry();
}

void renderer::swap_buffer(void* window) {
    GLenum gl_error;
    while ((gl_error = glGetError()) != GL_NO_ERROR)
        BOOST_LOG_TRIVIAL(error) << "OpenGL Error: " << gl_error;

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow((SDL_Window*)(window));
}

std::string renderer::get_gpu_name() const {
    std::string str = std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    auto split_at = str.find_first_of('(');

    if (split_at != str.npos)
        str.erase(str.begin() + (split_at - 1), str.end());

    return str;
}

std::string renderer::get_renderer_info() const {
    std::string str = std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    auto split_at = str.find_first_of('(');

    if (split_at != str.npos)
        str.erase(str.begin() + (split_at - 1), str.end());

    return "OpenGL " + str;
}

bool renderer::vsync_enabled() const {
    return SDL_GL_GetSwapInterval() == 1;
}

void renderer::set_vsync_enabled(const bool state) {
    SDL_GL_SetSwapInterval(state ? 1 : 0);
}

void renderer::ready_gui_draw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void renderer::end_gui_draw() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void renderer::render_geometry()
{
    if (!camera_exists())
        return;

    auto& registry = scene::singleton().get_registry();

    glEnable(GL_DEPTH_TEST);

    auto draw_view = registry.create_view<component::render_mesh, component::transform>();

    auto& camera = registry.get<component::camera>(get_active_camera());
    auto& camera_transf = registry.get<component::transform>(get_active_camera());

    // update camera uniform buffer
    {
        camera_ub camera_data;
        camera_data.projection = camera.get_view_matrix(m_aspect_ratio);
        camera_data.view = glm::inverse(camera_transf.get_global());
        glNamedBufferSubData(m_camera_ub, 0, sizeof(camera_ub), &camera_data);
    }

    // bind uniform buffer
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera_ub);

    m_default_shader->use();

    for (auto it = draw_view.begin(); it != draw_view.end(); ++it) {
        auto& drawable = it.get<component::render_mesh>();
        auto& transform = it.get<component::transform>();

        m_default_shader->set_transform(transform.get_global());

        for (auto m_it = drawable.meshes.begin(); m_it != drawable.meshes.end(); ++m_it)
        {
            const auto& skm = *m_it;

            if (!skm)
                continue;

            m_default_shader->set_material(skm.material);
            skm.mesh->draw();
        }
    }
}

} /* end of namespace tavern::graphics */

#endif // USE_OPENGL
