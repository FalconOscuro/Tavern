#include "tavern-bin/panels/scene.h"

#include <string>

#include <imgui.h>

#include <tavern/core/scene.h>

namespace panel {

void scene_p::draw()
{
    static tavern::scene& scene = tavern::scene::singleton();

    ImGui::Begin("Scene");

    ImGui::InputText("##", m_save_name, 64);

    ImGui::SameLine();

    if (ImGui::Button("Save") && strlen(m_save_name))
        scene.save(std::string(m_save_name));

    ImGui::End();
}

} /* end of namespace panel */
