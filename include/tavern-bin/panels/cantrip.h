#ifndef CANTRIP_PANEL_H
#define CANTRIP_PANEL_H

#include <tavern/graphics/imgui_panel.h>

namespace panel {

class cantrip_panel final : public tavern::graphics::imgui_panel
{
public:

    cantrip_panel() = default;
    ~cantrip_panel() = default;

    void draw() override;

private:

    bool m_open = false;

    char m_load_module_path[64] = {};
};

} /* namespace panel */

#endif /* end of include guard: CANTRIP_PANEL_H */
