#ifdef USE_OPENGL

#include "tavern/graphics/opengl/renderer.hpp"

#include <cassert>

#include <boost/log/trivial.hpp>

#include "tavern/core/window.h"

#include "tavern/components/drawable3d.h"
#include "tavern/components/transform3d.h"
#include "tavern/components/camera.h"

#include "tavern/resource/resource_manager.h"

namespace tavern::graphics::opengl {

void renderer::clean() {

    if (!m_glcontext)
        return;

    // release ownership of default shader
    m_default_shader.reset((shader*)nullptr);

    SDL_GL_DeleteContext(m_glcontext);
    BOOST_LOG_TRIVIAL(trace) << "Shutdown OpenGL";
    m_glcontext = NULL;
}

bool renderer::init(window& wnd) {

    // init opengl context
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

    set_viewport_size(wnd.get_size());

    m_default_shader = resource_manager::get().load_shader("shaders/basic.yml");

    return true;
}

void renderer::clear() {
    glClearColor(0.f, 0.f, .2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer::render(ecs::registry& registry)
{
    auto cam_view = registry.create_view<component::transform, component::camera>();

    // no valid camera, skip rendering
    if (cam_view.size() == 0)
        return;

    for (auto it = cam_view.begin(); it != cam_view.end(); ++it) {
        ecs::entity_type cam_id = *it;
        auto& camera = it.get<component::camera>();

        // active camera becomes inactive
        if (cam_id == m_camera && !camera.active)
            m_camera = UINT32_MAX;

        // discover new active camera
        else if (cam_id != m_camera && camera.active) {
            // unset previous camera active state
            // prioritize new cameras over current
            // avoid setting multiple cameras as active during a single frame, as outcome can be non-deterministic from user perspective
            if (registry.has<component::camera>(m_camera))
                registry.get<component::camera>(m_camera).active = false;

            m_camera = cam_id;
        }
    }

    // no camera to draw to, skip rendering
    if (!registry.has<component::transform, component::camera>(m_camera))
        return;

    auto draw_view = registry.create_view<component::drawable3d, component::transform>();

    auto& camera = registry.get<component::camera>(m_camera);
    auto& camera_transf = registry.get<component::transform>(m_camera);
    auto& shader = m_default_shader;

    shader->use();
    shader->set_mat4x4("projection", glm::perspective(glm::radians(camera.fov), m_aspect_ratio, camera.near, camera.far));
    shader->set_mat4x4("view", glm::inverse(camera_transf.get_global()));

    for (auto it = draw_view.begin(); it != draw_view.end(); ++it) {
        auto& drawable = it.get<component::drawable3d>();
        auto& transform = it.get<component::transform>();

        // TODO: Account for custom shaders
        //resource::shader_manager::resource_ptr& shader = drawable.shader ? drawable.shader : m_default_shader;

        shader->set_mat4x4("model", transform.get_global());

        drawable.mesh->use();
    }
}

void renderer::swap_buffer(window& wnd) {

    GLenum gl_error;
    while ((gl_error = glGetError()) != GL_NO_ERROR)
        BOOST_LOG_TRIVIAL(error) << "OpenGL Error: " << gl_error;

    SDL_GL_SwapWindow(wnd);
}

} /* end of namespace tavern::graphics */

#endif // USE_OPENGL
