#ifndef SCENE_PANEL_H
#define SCENE_PANEL_H

#include <tavern/graphics/imgui_panel.h>

#include <ecs/ecs.h>

namespace panel {

class scene_p final : public tavern::graphics::imgui_panel
{
public:

    scene_p() = default;
    ~scene_p() = default;

    void draw() override;

private:

    void draw_entity_tree();
    void component_edit();

    void edit_name();
    void edit_transform();

    char m_save_name[64];

    ecs::entity_type m_selected = (ecs::entity_type)-1;

}; /* end of class scene_p */

} /* end of namespace panel */

#endif /* end of define guard SCENE_PANEL_H */
