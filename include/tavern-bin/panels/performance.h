#ifndef PERFORMANCE_PANEL_H
#define PERFORMANCE_PANEL_H

#include <cstddef>

#include <tavern/graphics/imgui_panel.h>

#define NUM_FRAME_TIMES 600

namespace panel {

class performance : public tavern::graphics::imgui_panel
{
public:

    performance() = default;
    ~performance() = default;

    void draw() override;

private:

    float m_frame_times[NUM_FRAME_TIMES];
    size_t m_offset = 0;

}; /* end of class performance : public tavern::graphics::imgui_panel */

} /* end of namespace panel */

#endif /* end of define guard PERFORMANCE_PANEL_H */
