#ifndef FILE_SYSTEM_PANEL_H
#define FILE_SYSTEM_PANEL_H

#include <tavern/graphics/imgui_panel.h>

namespace panel {

class file_system_p final : public tavern::graphics::imgui_panel
{
public:

    file_system_p() = default;
    ~file_system_p() = default;

    void draw() override;

private:

    char m_dir_mount_identifier[32];
    char m_dir_mount_path[64];

    char m_tpk_mount_path[64];

    bool m_open = false;

}; /* end of class file_system_p final : public tavern::graphics::imgui_panel */

} /* end of namespace panel */

#endif /* end of define guard FILE_SYSTEM_PANEL_H */
