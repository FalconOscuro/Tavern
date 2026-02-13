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

    ImGui::InputText("##load cantrip module name", m_load_module_path, sizeof(m_load_module_path));
    ImGui::SameLine();


    if(ImGui::Button("Load") && strlen(m_load_module_path))
    {
        tavern::file::mount_path path;

        if (tavern::file::mount_path::try_create(m_load_module_path, path))
            cantrip.load_module(path);

        else
            BOOST_LOG_TRIVIAL(error) << "Cannot parse invalid mount path '" << m_load_module_path << '\'';
    }

    if (ImGui::Button("Unload All"))
        cantrip.unload_all_modules();

    // display list of all loaded modules
    // examine/modify individual modules
    // Load modules!!!

    ImGui::End();
}

} /* namespace panel */
