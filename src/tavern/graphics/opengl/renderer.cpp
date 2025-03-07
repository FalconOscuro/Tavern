#ifdef USE_OPENGL

#include "tavern/graphics/opengl/renderer.hpp"

#include <cassert>

#include <boost/log/trivial.hpp>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "tavern/tavern.h"

#include "tavern/core/window.h"

#include "tavern/components/drawable3d.h"
#include "tavern/components/transform3d.h"
#include "tavern/components/camera.h"

#include "tavern/resource/resource_manager.h"

#include "shaders/pbr.hpp"

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

void renderer::shutdown() {

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

bool renderer::init() {

    // init opengl context
    window& wnd = tavern::singleton().get_window();
    m_glcontext = SDL_GL_CreateContext(wnd);

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

    BOOST_LOG_TRIVIAL(info) << "Renderer: " << glGetString(GL_RENDERER);
    BOOST_LOG_TRIVIAL(info) << "OpenGL supported version: " << glGetString(GL_VERSION);

    BOOST_LOG_TRIVIAL(trace) << "Initialized OpenGL";

    // cull rear side of faces
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glFrontFace(GL_CCW);

    set_viewport_size(wnd.get_size());

    // load default shader
    // Check to ensure successful completion?
    m_default_shader = resource_manager::get().shaders.register_new("default", pbr::vert, pbr::frag);

    // create uniform buffer for camera data
    m_camera_ub = create_uniform_buffer<camera_ub>();

    // init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(wnd, m_glcontext);
    ImGui_ImplOpenGL3_Init();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    BOOST_LOG_TRIVIAL(trace) << "Initialized imgui";

    return true;
}

void renderer::render()
{
    imgui_draw();
    render_geometry();
}

void renderer::swap_buffer() {

    GLenum gl_error;
    while ((gl_error = glGetError()) != GL_NO_ERROR)
        BOOST_LOG_TRIVIAL(error) << "OpenGL Error: " << gl_error;

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(tavern::singleton().get_window());
}

void renderer::imgui_draw()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();

    {
        ImGui::Begin("Perfomance");
        ImGui::Text("Average: %.1f fps", io.Framerate);
        ImGui::End();
    }

    ImGui::Render();
}

void renderer::render_geometry()
{
    if (!camera_exists())
        return;

    auto& registry = tavern::singleton().get_registry();

    glEnable(GL_DEPTH_TEST);
    // clear screen
    glClearColor(0.f, 0.f, .2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto draw_view = registry.create_view<component::drawable3d, component::transform>();

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

    auto& shader = m_default_shader;
    shader->use();

    for (auto it = draw_view.begin(); it != draw_view.end(); ++it) {
        auto& drawable = it.get<component::drawable3d>();
        auto& transform = it.get<component::transform>();

        if (!drawable.mesh)
            continue;

        shader->set_transform(transform.get_global());
        shader->set_material(drawable.material);

        drawable.mesh->draw();
    }
}

void renderer::render_gui()
{
    // Need to posistion ui elements on screen

    // draw images
    // draw text
}

} /* end of namespace tavern::graphics */

#endif // USE_OPENGL
