#ifdef USE_OPENGL

#include "tavern/graphics/opengl_renderer.h"

#include <GL/glew.h>

#include <boost/log/trivial.hpp>

namespace tavern::graphics {

bool opengl_renderer::init() {

    // init glew
    glewExperimental = true;
    GLenum glew_error = glewInit();

    if (glew_error != GLEW_OK) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize GLEW: " << glewGetErrorString(glew_error);
        return false;
    }

    BOOST_LOG_TRIVIAL(info) << "Renderer: " << glGetString(GL_RENDERER);
    BOOST_LOG_TRIVIAL(info) << "OpenGL supported version: " << glGetString(GL_VERSION);

    BOOST_LOG_TRIVIAL(info) << "Initialized OpenGL";

    return true;
}

void opengl_renderer::update()
{}

} /* end of namespace tavern::graphics */

#endif // USE_OPENGL
