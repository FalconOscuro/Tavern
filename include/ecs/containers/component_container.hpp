#ifndef COMPONENT_CONTAINER_H
#define COMPONENT_CONTAINER_H

#include "../entity/entity.h"

namespace ecs::container {

template<typename component_type>
class component_container final
{
public:
    friend class sparse_map;

    // WARNING: should set id to null
    component_container() = default;

    component_container(entity_type entity_id, const component_type& component):
        entity_id(entity_id), component(component)
    {}

    ~component_container() = default;

    component_type& operator*() {
        return component;
    }

    const component_type& operator*() const {
        return component;
    }

    component_type* operator->() {
        return &component;
    }

    const component_type* operator->() const {
        return &component;
    }

private:
    entity_type entity_id;

public:

    component_type component;

    entity_type id() const {
        return entity_id;
    }
}; /* end of struct component_container */

} /* namespace ecs::container */

#endif /* end of include guard: COMPONENT_CONTAINER_H */
