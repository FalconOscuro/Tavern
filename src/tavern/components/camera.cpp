#include "tavern/components/camera.h"

namespace tavern::component {

bool read(const ryml::ConstNodeRef& n, camera* val)
{
    if (!n.is_map())
        return false;

    // camera could be hardcoded?
    n["fov"] >> val->fov;
    n["near"] >> val->near;
    n["far"] >> val->far;
    n["active"] >> val->active;
    
    return true;
}

void write(ryml::NodeRef* n, const camera& val)
{
    *n |= ryml::MAP;

    n->append_child() << ryml::key("fov") << val.fov;
    n->append_child() << ryml::key("near") << val.near;
    n->append_child() << ryml::key("far") << val.far;
    n->append_child() << ryml::key("active") << val.active;
}

} /* namespace tavern::component */
