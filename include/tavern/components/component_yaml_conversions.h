#ifndef COMPONENT_YAML_CONVERSIONS_H
#define COMPONENT_YAML_CONVERSIONS_H

#include <ryml.hpp>

#include "camera.h"
#include "transform.h"

#define TAVERN_TAG_DIRECTIVE "!t!"

namespace tavern::component {

bool read(const ryml::ConstNodeRef& n, camera* val);
void write(ryml::NodeRef* n, const camera& val);

// ryml read/write
bool read(const ryml::ConstNodeRef& n, transform* val);
void write(ryml::NodeRef* n, const transform& val);

} /* end of namespace tavern::component */

namespace glm {

bool read(const ryml::ConstNodeRef& n, mat4* val);
void write(ryml::NodeRef* n, const mat4& val);

bool read(const ryml::ConstNodeRef& n, vec4* val);
void write(ryml::NodeRef* n, const vec4& val);

bool read(const ryml::ConstNodeRef& n, vec3* val);
void write(ryml::NodeRef* n, const vec3& val);

} /* end of namespace glm */

#endif /* end of define guard COMPONENT_YAML_CONVERSIONS_H */
