#include "tavern/components/component_yaml_conversions.hpp"

#include <c4/yml/node.hpp>
#include <c4/yml/node_type.hpp>

#include <glm/gtc/quaternion.hpp>

#include <ryml_std.hpp>

#include "tavern/core/resource_manager.h"

namespace tavern::component {

bool read(const ryml::ConstNodeRef& n, camera* val)
{
    // camera could be hardcoded?
    // WARNING: Doesn't handle malformed data
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

    n->set_val_tag(component_type_info<camera>::c4_tag());
}

bool read(const ryml::ConstNodeRef &n, transform *val)
{
    if (!(n.has_child("position") && n.has_child("scale") && n.has_child("rotation")))
        return false;

    glm::vec3 position, rotation, scale;

    n["position"] >> position;
    n["rotation"] >> rotation;
    n["scale"] >> scale;

    const glm::mat4 p_mat = glm::translate(glm::mat4(1), position);
    const glm::mat4 r_mat = glm::mat4_cast(glm::quat(rotation));
    const glm::mat4 s_mat = glm::scale(glm::mat4(1), scale);

    val->local = p_mat * r_mat * s_mat;

    if (n.has_child("parent"))
        n["parent"] >> val->parent;

    else
        val->parent = ecs::entity_type(-1);

    return true;
}

void write(ryml::NodeRef *n, const transform &val)
{
    *n |= ryml::MAP;

    n->append_child() << ryml::key("position") << val.get_local_translate();
    n->append_child() << ryml::key("rotation") << val.get_local_euler();
    n->append_child() << ryml::key("scale") << val.get_local_scale();

    // currently handled externally
    //if (val.parent != ecs::entity_type(-1))
    //    n->append_child() << ryml::key("parent") << val.parent;

    n->set_val_tag(component_type_info<transform>::c4_tag());
}

bool read(const ryml::ConstNodeRef& n, skinned_mesh* val)
{
    auto& res_mngr = resource_manager::singleton();

    const auto mesh = n["mesh"];
    const auto material = n["material"];

    if (!(mesh.has_val() && mesh.val_is_null())) {
        file::mount_path path;
        mesh >> path;
        val->mesh = res_mngr.meshes.load(path);
    }

    if (!(material.has_val() && material.val_is_null())) {
        file::mount_path path;
        material >> path;
        val->material = res_mngr.materials.load(path);
    }

    return true;
}

void write(ryml::NodeRef* n, const skinned_mesh& val)
{
    *n |= ryml::MAP;

    auto mesh = n->append_child();
    mesh.set_key("mesh");

    if (val.mesh)
        mesh << val.mesh.get_path();
    else
        mesh << nullptr;

    auto material = n->append_child();
    material.set_key("material");

    if (val.material)
        material << val.material.get_path();
    else
        material << nullptr;
}

bool read(const ryml::ConstNodeRef& n, render_mesh* val)
{
    n["meshes"] >> val->meshes;

    return true;
}

void write(ryml::NodeRef* n, const render_mesh& val)
{
    *n |= ryml::MAP;

    auto meshes = n->append_child();
    meshes.set_key("meshes");
    meshes << val.meshes;

    n->set_val_tag(component_type_info<render_mesh>::c4_tag());
}

} /* end of namespace tavern::component */

namespace glm {

bool read(const ryml::ConstNodeRef &n, mat4 *val)
{
    for (unsigned int i = 0; i < 4; ++i)
        n[i] >> (*val)[i];

    return true;
}

void write(ryml::NodeRef* n, const mat4& val)
{
    *n |= ryml::SEQ;

    for (unsigned int i = 0; i < 4; ++i)
        n->append_child() << val[i];
}

bool read(const ryml::ConstNodeRef& n, vec4* val)
{
    if (n.num_children() != 4)
        return false;

    n["x"] >> val->x;
    n["y"] >> val->y;
    n["z"] >> val->z;
    n["w"] >> val->w;

    return true;
}

void write(ryml::NodeRef* n, const vec4& val)
{
    *n |= ryml::MAP | ryml::FLOW_SL;

    n->append_child() << ryml::key("x") << val.x;
    n->append_child() << ryml::key("y") << val.y;
    n->append_child() << ryml::key("z") << val.z;
    n->append_child() << ryml::key("w") << val.w;
}

bool read(const ryml::ConstNodeRef& n, vec3* val)
{
    if (n.num_children() != 3)
        return false;

    n["x"] >> val->x;
    n["y"] >> val->y;
    n["z"] >> val->z;

    return true;
}

void write(ryml::NodeRef* n, const vec3& val)
{
    *n |= ryml::MAP | ryml::FLOW_SL;

    n->append_child() << ryml::key("x") << val.x;
    n->append_child() << ryml::key("y") << val.y;
    n->append_child() << ryml::key("z") << val.z;
}

} /* end of namespace glm */
