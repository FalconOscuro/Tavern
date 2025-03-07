#include "tavern/graphics/generic/renderer.h"

#include <boost/log/trivial.hpp>

#include "tavern/tavern.h"

#include "tavern/components/camera.h"
#include "tavern/components/transform3d.h"

namespace tavern::graphics::generic {

void renderer::update() {
    update_camera();
}

bool renderer::camera_exists() const {
    const ecs::registry& registry = tavern::singleton().get_registry();
    
    // NOTE has should check for exitance first, causes error
    return registry.exists(m_camera) && registry.has<component::camera, component::transform>(m_camera);
}

void renderer::update_camera()
{
    ecs::registry& registry = tavern::singleton().get_registry();

    auto cam_view = registry.create_view<component::transform, component::camera>();

    // no valid camera, skip rendering
    if (cam_view.size() == 0) {
        no_camera_warning();
        return;
    }

    for (auto it = cam_view.begin(); it != cam_view.end(); ++it) {
        ecs::entity_type cam_id = *it;
        auto& camera = it.get<component::camera>();

        // active camera becomes inactive
        if (cam_id == m_camera && !camera.active)
            m_camera = UINT32_MAX;

        // discover new active camera
        else if (cam_id != m_camera && camera.active) {
            // unset previous camera active state
            // prioritize new cameras over current
            // avoid setting multiple cameras as active during a single frame, as outcome can be non-deterministic from user perspective
            if (registry.has<component::camera>(m_camera))
                registry.get<component::camera>(m_camera).active = false;

            m_camera = cam_id;
        }
    }

    no_camera_warning();
}

void renderer::no_camera_warning() const {
    // should only be one instance, enforce singleton
    static bool has_print_warn = false;

    if (camera_exists())
        has_print_warn = false;
    
    else if (!has_print_warn) {
        BOOST_LOG_TRIVIAL(warning) << "Failed to find valid camera in active scene!";
        has_print_warn = true;
    }
}

} /* end of namespace tavern::graphics::generic */
