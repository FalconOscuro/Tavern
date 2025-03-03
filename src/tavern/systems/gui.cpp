#include "tavern/systems/gui.h"

#include "tavern/gui/bounds.h"
#include "tavern/gui/container.h"

#include "tavern/tavern.h"
#include <glm/common.hpp>

namespace tavern::system {

void gui_sys::update()
{
    // need to update min sizes first
    update_containers();
    update_screen_coords();
}

void gui_sys::update_containers()
{
    auto& reg = tavern::singleton().get_registry();

    // 1. Update parent/child relations
    auto containers = reg.create_view<gui::container, gui::bounds>();

    for (auto it = containers.begin(); it != containers.end(); ++it) 
    {
        auto& container = it.get<gui::container>();

        for (auto child = container.children.begin(); child != container.children.end();) {

            // remove invalid ranges of children where either:
            // a) child refers to self
            // b) child bounds component non-existant
            if (*child == *it || !reg.has<gui::bounds>(*child))
            {
                auto first = child;
                do {
                    ++child;
                } while (*child == *it || !reg.has<gui::bounds>(*child)); // repeated condition

                child = container.children.erase(first, child);

                continue;
            }

            // set parent
            reg.get<gui::bounds>(*child).m_parent = *it;
            ++child;
        }
    }

    // update screen pos/size
}

void gui_sys::update_screen_coords()
{
    glm::ivec2 wnd_size = tavern::singleton().get_window().get_size();
    glm::ivec2 wnd_pos = glm::ivec2(0);
    auto& reg = tavern::singleton().get_registry();

    // don't need view as single arg
    auto gui_objs = reg.get_pool<gui::bounds>();

    for (auto it = gui_objs.begin(); it != gui_objs.end(); ++it) {
        auto& bound = it->first;

        // skip all objects with parents, positioning controlled by parent container
        // WARNING: Possibility of child ownership being removed without bounds knowledge causing possible leak
        if (bound.m_parent >= reg.tombstone() || !reg.has<gui::container>(bound.m_parent))
            bound.m_parent = reg.tombstone();

        else
            update_bound(it->second, bound, wnd_pos, wnd_size, reg);
    }
}

glm::vec2 gui_sys::update_bound(const ecs::entity_type eid, gui::bounds& bound, const glm::vec2 container_pos, const glm::vec2 container_size, ecs::registry& reg)
{

    // Sizing
    // X
    if ((bound.sizing & gui::bounds::X_STRETCH_PREFER_MIN) == gui::bounds::X_STRETCH_PREFER_MIN) {
        // does not update screen space coords for size
        bound.m_screen_size.x = glm::max(container_size.x, static_cast<float>(bound.get_min_size().x));
    }

    else if (bound.sizing & gui::bounds::X_STRETCH) {
        bound.m_screen_size.x = container_size.x;
    }

    else if (bound.sizing & gui::bounds::X_AUTO) {
        bound.m_screen_size.x = bound.get_min_size().x;
    }

    else {
        bound.m_screen_size.x = bound.size.x * container_size.x;
    }

    // Y
    if ((bound.sizing & gui::bounds::Y_STRETCH_PREFER_MIN) == gui::bounds::Y_STRETCH_PREFER_MIN) {
        // does not update screen space coords for size
        bound.m_screen_size.y = glm::max(container_size.y, static_cast<float>(bound.get_min_size().y));
    }

    else if (bound.sizing & gui::bounds::Y_STRETCH) {
        bound.m_screen_size.y = container_size.y;
    }

    else if (bound.sizing & gui::bounds::Y_AUTO) {
        bound.m_screen_size.y = bound.get_min_size().y;
    }

    else {
        bound.m_screen_size.y = bound.size.y * container_size.y;
    }

    // positioning:
    // X
    if ((bound.anchor & gui::bounds::HORIZONTAL_CENTER) == gui::bounds::HORIZONTAL_CENTER) {
        bound.m_screen_pos.x = (container_size.x - bound.m_screen_size.x) *.5f;
    }

    else if (bound.anchor & gui::bounds::LEFT) {
        // origin for all gui objects should be their top left
        bound.m_screen_pos.x = 0;
    }

    else if (bound.anchor & gui::bounds::RIGHT) {
        bound.m_screen_pos.x = container_size.x - bound.m_screen_size.x;
    }

    else {
        bound.m_screen_pos.x = container_size.x * bound.pos.x;
    }

    //Y
    if ((bound.anchor & gui::bounds::VERTICAL_CENTER) == gui::bounds::VERTICAL_CENTER) {
        bound.m_screen_pos.y = (container_size.y - bound.m_screen_size.y) *.5f;
    }

    else if (bound.anchor & gui::bounds::TOP) {
        // origin for all gui objects should be their top left
        bound.m_screen_pos.y = 0;
    }

    else if (bound.anchor & gui::bounds::BOTTOM) {
        bound.m_screen_pos.y = container_size.y - bound.m_screen_size.y;
    }

    else {
        bound.m_screen_pos.y = container_size.y * bound.pos.y;
    }

    // offset from container start
    bound.m_screen_pos += container_pos;

    // continue on to update children if is container entity
    if (!reg.has<gui::container>(eid))
        return bound.m_screen_size;

    auto& container = reg.get<gui::container>(eid);

    // stack containers have no additional positioning beyond passing on posision
    // vertical/horizontal containers need to assign based on which children can stretch and which are fixed in size 

    for (auto it = container.children.begin(); it != container.children.end();)
    {
        // should never fail, update containers should remove any invalid eids
        auto& child = reg.get<gui::bounds>(*it);

        // edge case, check if another entity has claimed ownership
        if (child.m_parent != eid) {
            continue;
            it = container.children.erase(it);
        }

        else
            ++it;
    }

    return bound.m_screen_size;
}

} /* end of namespace tavern::system */
