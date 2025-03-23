#include "tavern-bin/panels/scene.h"

#include <string>

#include <imgui.h>

#include <tavern/core/scene.h>

namespace panel {

void scene_p::draw()
{
    static tavern::scene& scene = tavern::scene::singleton();

    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

    const ImVec2 size = ImVec2(300, main_viewport->Size.y);
    ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - size.x, 0));
    ImGui::SetNextWindowSize(size);

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Scene", nullptr, flags);

    ImGui::BeginChild("Entities", ImVec2(size.x, ImGui::GetContentRegionAvail().y * .7));
    draw_entity_tree();
    ImGui::EndChild();

    ImGui::Separator();

    const char* component_items[] = { "name", "transform", "render mesh" };

    if (ImGui::BeginCombo("##component selector", ""))
    {
        for (size_t i = 0; i < IM_ARRAYSIZE(component_items); ++i)
        {
             ImGui::Selectable(component_items[i], false);
        }

        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::Button("Add");

    ImGui::Separator();

    ImGui::InputText("##save file name", m_save_name, 64);

    ImGui::SameLine();
    if (ImGui::Button("Save") && strlen(m_save_name))
        scene.save(std::string(m_save_name));

    ImGui::End();
}

void scene_p::draw_entity_tree()
{
    static tavern::scene& scene    = tavern::scene::singleton();
    static ecs::registry& registry = scene.get_registry();

    for (auto it = registry.entities_begin(); it != registry.entities_end(); ++it)
    {
        ImGui::TreeNodeEx((void*)(intptr_t)(*it), ImGuiTreeNodeFlags_Leaf, "%i", *it);
        ImGui::TreePop();
    }
}

} /* end of namespace panel */
