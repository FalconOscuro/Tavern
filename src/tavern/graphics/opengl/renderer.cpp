#ifdef USE_OPENGL

#include "tavern/graphics/opengl/renderer.hpp"

#include <cassert>

#include <boost/log/trivial.hpp>

#include "tavern/core/window.h"
#include "tavern/components/drawable3d.h"

namespace tavern::graphics::opengl {

void renderer::clean() {

    if (!m_glcontext)
        return;

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

    return true;
}

void renderer::clear() {
    glClearColor(0.f, 0.f, .2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer::render(ecs::registry& registry) {

    auto view = registry.create_view<component::drawable3d>();

    for (auto it = view.begin(); it != view.end(); it++) {
        auto& drawable = it.get<component::drawable3d>();

        drawable.Shader->use();
        drawable.Mesh->use();
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
