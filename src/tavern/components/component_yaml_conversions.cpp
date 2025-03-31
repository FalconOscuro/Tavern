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

    n->set_val_tag(get_type_tag<camera>());
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

    n->set_val_tag(get_type_tag<transform>());
}

bool read(const ryml::ConstNodeRef& n, render_mesh* val)
{
    auto& res_mngr = resource_manager::singleton();

    // should be better way of doing this
    if (n.has_child("mesh") && !n["mesh"].val().empty()) {
        file::mount_path path;
        n["mesh"] >> path;

        val->mesh = res_mngr.meshes.load(path);
    }

    if (n.has_child("material") && !n["material"].val().empty()) {
        file::mount_path path;
        n["material"] >> path;

        val->material = res_mngr.materials.load(path);
    }

    if (n.has_child("shader") && !n["shader"].val().empty()) {
        file::mount_path path;
        n["shader"] >> path;

        val->shader = res_mngr.shaders.load(path);
    }

    return true;
}

void write(ryml::NodeRef* n, const render_mesh& val)
{
    *n |= ryml::MAP;

    if (val.mesh)
        n->append_child() << ryml::key("mesh") << val.mesh.get_path();

    if (val.material)
        n->append_child() << ryml::key("material") << val.material.get_path();

    if (val.shader)
        n->append_child() << ryml::key("shader") << val.shader.get_path();

    n->set_val_tag(get_type_tag<render_mesh>());
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
