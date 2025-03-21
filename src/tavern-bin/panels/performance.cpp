#include "tavern-bin/panels/performance.h"

#include <imgui.h>

#include <tavern/core/window.h>
#include <tavern/graphics/renderer.h>
#include <tavern/core/input.h>

namespace panel {

void performance::draw()
{
    if (tavern::input::singleton().is_key_just_pressed("f8"))
        m_open = !m_open;

    if (!m_open)
        return;

    auto& io = ImGui::GetIO();

    const tavern::window& window = tavern::window::singleton();
    tavern::graphics::renderer& renderer = tavern::graphics::renderer::singleton();

    ImGui::Begin("Performance");

    m_frame_times[m_offset] = io.DeltaTime * 1000;
    m_offset = (m_offset + 1) % NUM_FRAME_TIMES;

    ImGui::PlotLines("Frame times (ms)", m_frame_times, NUM_FRAME_TIMES, m_offset);

    ImGui::Text("Framerate avg: %.1f fps", io.Framerate);

    ImGui::Separator();
    glm::ivec2 wnd_size = window.get_size();
    ImGui::Text("Window dimensions: %i x %i", wnd_size.x, wnd_size.y);

    ImGui::Text("GPU: %s", renderer.get_gpu_name().c_str());
    ImGui::Text("Renderer: %s", renderer.get_renderer_info().c_str());

    ImGui::Separator();
    bool vsync_enable = renderer.vsync_enabled();
    if (ImGui::Checkbox("Vsync", &vsync_enable)) {
        renderer.set_vsync_enabled(vsync_enable);
    }

    if (!renderer.camera_exists())
        ImGui::TextColored(ImVec4(227.f / 255.f, 146.f / 255.f, 6.f / 255.f, 1), "No camera found in scene!");

    ImGui::End();
}

} /* end of namespace panel */
