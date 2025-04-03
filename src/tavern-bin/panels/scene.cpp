#include "tavern-bin/panels/scene.h"

#include <string>

#include <imgui.h>

#include <tavern/core/scene.h>
#include <tavern/core/resource_manager.h>
#include <tavern/components/component.h>

#include <boost/log/trivial.hpp>

namespace panel {

// unsafe?
tavern::scene& g_scene    = tavern::scene::singleton();
ecs::registry& g_registry = g_scene.get_registry();

enum component_type_ids : size_t {
    NAME,
    TRANSFORM,
    RENDER_MESH,
    CAMERA
};

const char* component_names[] = {
    "name",
    "transform",
    "render mesh",
    "camera"
};

void scene_p::draw()
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

    const ImVec2 size = ImVec2(300, main_viewport->Size.y);
    ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - size.x, 0));
    ImGui::SetNextWindowSize(size);

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Scene", nullptr, flags);

    if (ImGui::Button("Create Entity"))
        m_selected = g_registry.create();

    ImGui::SameLine();

    if (ImGui::Button("Delete Entity"))
        g_registry.destroy(m_selected);

    if (ImGui::Button("Delete All"))
        g_registry.destroy_all();

    ImGui::Separator();

    ImGui::BeginChild("Entities", ImVec2(size.x, ImGui::GetContentRegionAvail().y * .4));
    draw_entity_tree();
    ImGui::EndChild();

    ImGui::Separator();
    component_edit();
    ImGui::Separator();

    ImGui::InputText("##save file name", m_save_name, 64);

    if (ImGui::Button("Save") && strlen(m_save_name))
        g_scene.save(std::string(m_save_name));

    ImGui::SameLine();
    if (ImGui::Button("Load") && strlen(m_save_name))
        g_scene.load(std::string(m_save_name));

    ImGui::End();
}

void scene_p::draw_entity_tree()
{
    for (auto it = g_registry.entities_begin(); it != g_registry.entities_end(); ++it)
    {
        int flags = 0;

        char name_buf[ENTITY_NAME_MAX_LEN + 64]{'\0'};

        const auto* name_comp = g_registry.try_get<tavern::component::entity_name>(*it);
        if (name_comp && strlen(name_comp->get()))
            sprintf(name_buf, "%s (%i)", name_comp->get(), *it);

        else
            sprintf(name_buf, "%i", *it);

        if (*it == m_selected)
            flags |= ImGuiTreeNodeFlags_Selected;

        // no children
        flags |= ImGuiTreeNodeFlags_Leaf;

        ImGui::TreeNodeEx((void*)(intptr_t)(*it), flags, "%s", name_buf);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            m_selected = *it;

        ImGui::TreePop();
    }
}

template <typename T>
inline void add_component(ecs::entity_type entity)
{
    if (!g_registry.has<T>(entity))
        g_registry.emplace<T>(entity);
}

void scene_p::component_edit()
{
    static component_type_ids selected_component;

    if (ImGui::BeginCombo("##component selector", component_names[selected_component]))
    {
        for (size_t i = 0; i < IM_ARRAYSIZE(component_names); ++i)
             if (ImGui::Selectable(component_names[i], i == selected_component))
                selected_component = (component_type_ids)i;

        ImGui::EndCombo();
    }

    ImGui::SameLine();

    if (ImGui::Button("Add") && g_registry.exists(m_selected))
    {
        switch (selected_component)
        {
        // name
        case NAME:
            add_component<tavern::component::entity_name>(m_selected);
            break;

        case TRANSFORM:
            add_component<tavern::component::transform>(m_selected);
            break;

        case RENDER_MESH:
            add_component<tavern::component::render_mesh>(m_selected);
            break;

        case CAMERA:
            add_component<tavern::component::camera>(m_selected);
            break;

        default:
            BOOST_LOG_TRIVIAL(warning) << "Unrecognized component id type: " << selected_component;
            break;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Remove"))
    {
        switch (selected_component)
        {
        // name
        case NAME:
            g_registry.remove<tavern::component::entity_name>(m_selected);
            break;

        case TRANSFORM:
            g_registry.remove<tavern::component::transform>(m_selected);
            break;

        case CAMERA:
            g_registry.remove<tavern::component::camera>(m_selected);
            break;

        case RENDER_MESH:
            g_registry.remove<tavern::component::render_mesh>(m_selected);
            break;

        default:
            BOOST_LOG_TRIVIAL(warning) << "Unrecognized component id type: " << selected_component;
            break;
        }
    }

    ImGui::Separator();

    if (!g_registry.exists(m_selected)) {
        ImGui::Text("No entity selected");
        return;
    }

    switch (selected_component)
    {
    // name
    case NAME:
        edit_name();
        break;

    case TRANSFORM:
        edit_transform();
        break;

    case CAMERA:
        if (g_registry.has<tavern::component::camera, tavern::component::transform>(m_selected))
            ImGui::Text("Camera Valid");
        break;

    case RENDER_MESH:
        edit_render_mesh();
        break;

    default:
        BOOST_LOG_TRIVIAL(warning) << "Unrecognized component id type: " << selected_component;
        break;
    }
}

void scene_p::edit_name()
{
    if (!g_registry.has<tavern::component::entity_name>(m_selected)) {
        ImGui::Text("Entity name component is not attached");
        return;
    }

    auto& name = g_registry.get<tavern::component::entity_name>(m_selected);

    ImGui::InputText("##edit name", *name, ENTITY_NAME_MAX_LEN);
}

bool edit_vec3(glm::vec3* v, const char* id, ImGuiInputTextFlags flags = 0)
{
    ImGui::PushID(id);
    bool changed = false;
    flags |= ImGuiInputTextFlags_ParseEmptyRefVal | ImGuiInputTextFlags_NoUndoRedo;

    const float width = ImGui::GetContentRegionAvail().x / 1.5 / 3;

    ImGui::Text("X");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(width);
    changed |= ImGui::InputFloat("##X", &(v->x), 0.0f, 0.0f, "%.3f", flags);

    ImGui::SameLine();
    ImGui::Text("Y");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(width);
    changed |= ImGui::InputFloat("##Y", &(v->y), 0.0f, 0.0f, "%.3f", flags);

    ImGui::SameLine();
    ImGui::Text("Z");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(width);
    changed |= ImGui::InputFloat("##Z", &(v->z), 0.0f, 0.0f, "%.3f", flags);

    ImGui::PopID();

    return changed;
}

void scene_p::edit_transform()
{
    if (!g_registry.has<tavern::component::transform>(m_selected)) {
        ImGui::Text("Transform component is not attached");
        return;
    }

    auto& transform = g_registry.get<tavern::component::transform>(m_selected);

    {
        bool changed = false;

        ImGui::PushID("Local");
        ImGui::Text("Local");

        glm::vec3 pos = transform.get_local_translate();
        glm::vec3 rot = glm::degrees(transform.get_local_euler());
        glm::vec3 scl = transform.get_local_scale();

        ImGui::Text("Position:");
        ImGui::SameLine();
        changed |= edit_vec3(&pos, "pos");

        ImGui::Text("Rotation:");
        ImGui::SameLine();
        changed |= edit_vec3(&rot, "rot");
        
        ImGui::Text("Scale:   ");
        ImGui::SameLine();
        // scale should never be 0
        changed |= edit_vec3(&scl, "scl") && scl.x && scl.y && scl.z;

        ImGui::PopID();

        if (changed) {
            const glm::mat4 p_mat = glm::translate(glm::mat4(1), pos);
            const glm::mat4 r_mat = glm::mat4_cast(glm::quat(glm::radians(rot)));
            const glm::mat4 s_mat = glm::scale(glm::mat4(1), scl);

            transform.local = p_mat * r_mat * s_mat;
        }
    }

    {
        ImGui::PushID("Global");
        ImGui::Text("Global");

        glm::vec3 global_pos = transform.get_global_translate();
        glm::vec3 global_rot = glm::degrees(transform.get_global_euler());
        glm::vec3 global_scl = transform.get_global_scale();

        ImGui::Text("Position:");
        ImGui::SameLine();
        edit_vec3(&global_pos, "global_pos", ImGuiInputTextFlags_ReadOnly);

        ImGui::Text("Rotation:");
        ImGui::SameLine();
        edit_vec3(&global_rot, "global_rot", ImGuiInputTextFlags_ReadOnly);

        ImGui::Text("Scale:   ");
        ImGui::SameLine();
        edit_vec3(&global_scl, "global_scale", ImGuiInputTextFlags_ReadOnly);

        ImGui::PopID();
    }

    auto parent = transform.parent;

    ImGui::Text("Parent:");
    ImGui::SameLine();
    if (
        ImGui::InputScalar("##parentid", ImGuiDataType_U32, &parent)
        && (
            parent >= g_registry.tombstone()
            || ( parent != m_selected
            && g_registry.has<tavern::component::transform>(parent)
            )
        )
    ) {
        transform.parent = parent;
    }
}

void scene_p::edit_render_mesh()
{
    if (!g_registry.has<tavern::component::render_mesh>(m_selected)) {
        ImGui::Text("Render Mesh component is not attached");
        return;
    }

    auto& render_mesh = g_registry.get<tavern::component::render_mesh>(m_selected);

    if (!g_registry.has<tavern::component::transform>(m_selected))
        ImGui::TextColored(ImVec4(227.f / 255.f, 146.f / 255.f, 6.f / 255.f, 1), "Render mesh is missing a transform so will not be drawn");

    static char mesh_name[64];
    ImGui::InputText("Mesh Resource", mesh_name, IM_ARRAYSIZE(mesh_name));

    if (ImGui::Button("Apply##mesh"))
    {
        tavern::file::mount_path path;
        if (tavern::file::mount_path::try_create(mesh_name, path))
            render_mesh.mesh = tavern::resource_manager::singleton().meshes.load(path);

        mesh_name[0] = '\0';
    }

    static char material_name[64];
    ImGui::InputText("Material Resource", material_name, IM_ARRAYSIZE(material_name));

    if (ImGui::Button("Apply##material"))
    {
        tavern::file::mount_path path;
        if (tavern::file::mount_path::try_create(material_name, path))
            render_mesh.material = tavern::resource_manager::singleton().materials.load(path);

        material_name[0] = '\0';
    }
}

} /* end of namespace panel */
