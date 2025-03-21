#include "tavern/components/component_yaml_conversions.h"

#include <c4/yml/node.hpp>
#include <c4/yml/node_type.hpp>

#include <glm/gtc/quaternion.hpp>

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

    n->append_child() << ryml::key("position") << val.get_global_translate();
    n->append_child() << ryml::key("rotation") << val.get_local_euler();
    n->append_child() << ryml::key("scale") << val.get_local_scale();

    if (val.parent != ecs::entity_type(-1))
        n->append_child() << ryml::key("parent") << val.parent;
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

    for (unsigned int i = 0; i < 4; ++i)
        n[i] >> (*val)[i];

    return true;
}

void write(ryml::NodeRef* n, const vec4& val)
{
    *n |= ryml::SEQ | ryml::FLOW_SL;

    for (unsigned int i = 0; i < 4; ++i)
        n->append_child() << val[i];
}

bool read(const ryml::ConstNodeRef& n, vec3* val)
{
    if (n.num_children() != 3)
        return false;

    for (size_t i = 0; i < 3; ++i)
         n[i] >> (*val)[i];

    return true;
}

void write(ryml::NodeRef* n, const vec3& val)
{
    *n |= ryml::SEQ | ryml::FLOW_SL;

    for (size_t i = 0; i < 3; ++i)
        n->append_child() << val[i];
}

} /* end of namespace glm */
