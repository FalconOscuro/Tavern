#ifndef IMGUI_PANEL_H
#define IMGUI_PANEL_H

namespace tavern::graphics {

class imgui_panel
{
public:

    virtual ~imgui_panel() = default;

    virtual void draw() = 0;

}; /* end of class imgui_panel */

} /* end of namespace tavern::graphics */

#endif /* end of define guard IMGUI_PANEL_H */
