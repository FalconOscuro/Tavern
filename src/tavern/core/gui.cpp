#include "tavern/core/gui.h"

#include <boost/log/trivial.hpp>

#include <RmlUi/Debugger.h>

namespace tavern {

bool gui::init(const glm::ivec2& size)
{
    if (m_initialized)
        return true;

    Rml::SetRenderInterface(&m_render_interface);

    if (!Rml::Initialise()) {
        BOOST_LOG_TRIVIAL(error) << "Failed to initialize Rml";
        return false;
    }

    m_context = Rml::CreateContext("main", Rml::Vector2i(size.x, size.y));

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
    m_context->SetDimensions(Rml::Vector2i(size.x, size.y));
    m_render_interface.resize(size);
}

bool gui::load_document(const char* path) {
    Rml::ElementDocument* document = m_context->LoadDocument(path);

    if (document)
        document->Show();

    return document;
}

} /* end of namespace tavern */
