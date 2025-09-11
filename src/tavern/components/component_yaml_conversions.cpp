#include "tavern/components/component_yaml_conversions.hpp"

#include <algorithm>
#include <boost/log/trivial.hpp>

#include <c4/yml/node.hpp>
#include <c4/yml/node_type.hpp>

#include <glm/gtc/quaternion.hpp>

#include <ryml_std.hpp>

#include "tavern/core/resource_manager.h"

namespace tavern::file {

// change to iterate over children to identify invalid args?

bool read(const ryml::ConstNodeRef& n, mount_path* val)
{
    if (!n.is_map())
        return false;

    std::string identifier, path;

    if (!(n.get_if("identifier", &identifier) && n.get_if("path", &path)))
    {
        BOOST_LOG_TRIVIAL(error) << "Malformed mount path whilst deserializing yaml node '" << n.id() << "' must have both 'identifier' and 'path' child nodes.";
        return false;
    }

    *val = mount_path(identifier, path);

    return true;
}

void write(ryml::NodeRef* n, const mount_path& val)
{
    *n |= ryml::MAP;
    n->append_child() << ryml::key("identifier") << std::string(val.get_identifier());
    n->append_child() << ryml::key("path") << std::string(val.get_path());
}

} /* end of namespace tavern::file */

namespace tavern::component {

bool read(const ryml::ConstNodeRef& n, camera* val)
{
    if (!n.is_map())
        return false;

    *val = camera();

    // camera could be hardcoded?
    // WARNING: Doesn't handle malformed data

    n.get_if("fov",     &val->fov);
    n.get_if("near",    &val->near);
    n.get_if("far",     &val->far);
    n.get_if("active",  &val->active);

    if (val->near >= val->far)
    {
        BOOST_LOG_TRIVIAL(error) << "Malformed camera component whilst deserializing yaml node '" << n.id() << "' near plane should be less than far plane.";
        return false;
    }
    
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
    *val = transform();

    // map of pos, scale and rot
    if (!n.is_map())
        return false;

    // allow for pure matrices?
    glm::vec3 position, rotation, scale;

    // should throw error or warning
    n.get_if("position", &position);
    n.get_if("rotation", &rotation); // fallback to quaternion?

    if (!n.get_if("scale", &scale))
        scale = glm::vec3(1.0f); // scale should default to 1

    const glm::mat4 p_mat = glm::translate(glm::mat4(1), position);
    const glm::mat4 r_mat = glm::mat4_cast(glm::quat(rotation));
    const glm::mat4 s_mat = glm::scale(glm::mat4(1), scale);

    val->local = p_mat * r_mat * s_mat;
    n.get_if("parent", &val->parent);

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
    // should only be one?
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
    if (!(n.is_seq() && n.num_children() >= 4))
        return false;

    for (unsigned int i = 0; i < 4; ++i)
        n[i] >> (*val)[i];

    return true;
}

void write(ryml::NodeRef* n, const mat4& val)
{
    *n |= ryml::SEQ;

    for (unsigned int i = 0; i < 4; ++i)
    {
        auto c = n->append_child();
        c |= ryml::SEQ;

        for (unsigned int j = 0; j < 4; ++j)
            c.append_child() << val[i][j];
    }
}

bool read(const ryml::ConstNodeRef& n, vec4* val)
{
    *val = vec4(0.f);

    if (n.is_map())
    {
        n.get_if("x", &val->x);
        n.get_if("y", &val->y);
        n.get_if("z", &val->z);
        n.get_if("w", &val->w);
    }

    else if (n.is_seq())
    {
        for (unsigned int i = 0; i < std::min<unsigned int>(4, n.num_children()); ++i)
            n[i] >> (*val)[i];
    }

    else return false;

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
    *val = vec3();

    if (n.is_map())
    {
        n.get_if("x", &val->x);
        n.get_if("y", &val->y);
        n.get_if("z", &val->z);
    }

    else if (n.is_seq())
    {
        for (unsigned int i = 0; i < std::min<unsigned int>(3, n.num_children()); ++i)
            n[i] >> (*val)[i];
    }

    else return false;

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
