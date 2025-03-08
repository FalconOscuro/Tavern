#include "tavern/core/gui.h"

#include <boost/log/trivial.hpp>

#include <RmlUi/Debugger.h>

#include "tavern/tavern.h"

namespace tavern {

bool gui::init()
{
    if (m_initialized)
        return true;

    Rml::SetRenderInterface(&m_render_interface);

    if (!Rml::Initialise()) {
        BOOST_LOG_TRIVIAL(error) << "Failed to initialize Rml";
        return false;
    }

    const glm::ivec2 screen_size = tavern::singleton().get_window().get_size();
    m_context = Rml::CreateContext("main", Rml::Vector2i(screen_size.x, screen_size.y));

    if (!m_context) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create Rml context";
        return false;
    }

    BOOST_LOG_TRIVIAL(trace) << "Rml(gui) initialization successful";
    return m_initialized = true;
}

void gui::shutdown() {

    if (!m_initialized)
        return;

    m_initialized = false;
    Rml::Shutdown();
    m_context = nullptr;

    BOOST_LOG_TRIVIAL(trace) << "Shutdown Rml(gui)";
}

void gui::update() {
    // check if init?
    m_context->Update();
}

void gui::render() {
    // check if init?
    m_context->Render();
}

void gui::resize(const glm::ivec2& size) {
    m_render_interface.resize(size);
}

} /* end of namespace tavern */
