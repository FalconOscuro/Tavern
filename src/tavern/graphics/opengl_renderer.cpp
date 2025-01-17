#ifdef USE_OPENGL

#include "tavern/graphics/opengl_renderer.hpp"

#include <boost/log/trivial.hpp>

namespace tavern::graphics {

void opengl_renderer::clean() {

    if (!m_glcontext)
        return;

    SDL_GL_DeleteContext(m_glcontext);
    BOOST_LOG_TRIVIAL(trace) << "Shutdown OpenGL";
    m_glcontext = NULL;
}

bool opengl_renderer::post_window_init(SDL_Window* wnd) {

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

    return true;
}

void opengl_renderer::update()
{}

void opengl_renderer::swap_buffer(SDL_Window* wnd) {

    GLenum gl_error;
    while ((gl_error = glGetError()) != GL_NO_ERROR)
        BOOST_LOG_TRIVIAL(error) << "OpenGL Error: " << gl_error;

    SDL_GL_SwapWindow(wnd);
}

void opengl_renderer::clear() {
    glClearColor(0.f, 0.f, .2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

} /* end of namespace tavern::graphics */

#endif // USE_OPENGL
