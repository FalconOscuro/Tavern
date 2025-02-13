#include "tavern/components/transform3d.h"

namespace tavern::component {

bool read(const ryml::ConstNodeRef &n, transform *val) {
    // NOTE: parent id needs handling externally
    // WARNING: Doesn't handle malformed data
    n["local"] >> val->local;

    return true;
}

void write(ryml::NodeRef *n, const transform &val)
{
    *n |= ryml::MAP;

    n->append_child() << ryml::key("local") << val.local;
    n->append_child() << ryml::key("parent") << val.parent;
}

} /* namespace tavern::components */

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

} /* namespace glm */
