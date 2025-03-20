#include "tavern-bin/panels/performance.h"

#include <imgui.h>

namespace panel {

void performance::draw()
{
    auto& io = ImGui::GetIO();

    ImGui::Begin("Performance");

    m_frame_times[m_offset] = io.DeltaTime * 1000;
    m_offset = (m_offset + 1) % NUM_FRAME_TIMES;

    ImGui::PlotLines("Frame times (ms)", m_frame_times, NUM_FRAME_TIMES, m_offset);

    ImGui::Text("Framerate avg: %.1f fps", io.Framerate);

    ImGui::End();
}

} /* end of namespace panel */
