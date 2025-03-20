#include "tavern/components/render_mesh.h"

#include <ryml_std.hpp>

#include "tavern/resource/resource_manager.h"

namespace tavern::component {

bool read(const ryml::ConstNodeRef& n, render_mesh* val)
{
    auto& res_mngr = resource_manager::get();

    val->mesh = res_mngr.meshes.load(n["mesh"].val().data());
    val->shader = res_mngr.shaders.load(n["shader"].val().data());

    return true;
}

void write(ryml::NodeRef* n, const render_mesh& val)
{
    *n |= ryml::MAP;

    n->append_child() << ryml::key("mesh") << val.mesh.get_name();
    n->append_child() << ryml::key("shader") << val.shader.get_name();
}

} /* end of namespace tavern::component */
