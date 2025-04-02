#include "tavern-bin/panels/file_system.h"

#include <tavern/core/file_system.h>
#include <tavern/core/input.h>

#include <imgui.h>

#include <boost/log/trivial.hpp>

namespace panel {

void file_system_p::draw()
{
    static auto& file_system = tavern::file_system::singleton();

    if (tavern::input::singleton().is_key_just_pressed("f7"))
        m_open = !m_open;

    if (!m_open)
        return;

    ImGui::Begin("File System");

    if (ImGui::TreeNode("Mount Directory"))
    {
        ImGui::InputText("Identifer", m_dir_mount_identifier, sizeof(m_dir_mount_identifier) - 1);
        ImGui::InputText("Path", m_dir_mount_path, sizeof(m_dir_mount_path) - 1);

        if (ImGui::Button("Mount"))
        {
            auto mount_info = tavern::file::mount_path(std::string(m_dir_mount_identifier), std::string(m_dir_mount_path));

            BOOST_LOG_TRIVIAL(trace) << std::string(m_dir_mount_path);

            file_system.mount_dir(mount_info);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Mount TPK"))
    {
        ImGui::InputText("File", m_tpk_mount_path, sizeof(m_tpk_mount_path) - 1);

        if (ImGui::Button("Mount"))
        {
            std::string ident;
            file_system.mount_tpk(std::string(m_tpk_mount_path), ident);
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

} /* end of namespace panel */
