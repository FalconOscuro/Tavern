#include "tavern-bin/panels/cantrip.h"

#include <tavern/core/cantrip.h>
#include <tavern/core/input.h>

#include <imgui.h>

#include <boost/log/trivial.hpp>

namespace panel {

void cantrip_panel::draw()
{
    static auto& cantrip = tavern::cantrip_modules::singleton();

    if (tavern::input::singleton().is_key_just_pressed("f9"))
        m_open = !m_open;

    if (!m_open)
        return;

    ImGui::Begin("Cantrip");

    if (ImGui::Button("Unload All"))
        cantrip.unload_all_modules();

    // display list of all loaded modules
    // examine/modify individual modules

    ImGui::End();
}

} /* namespace panel */
