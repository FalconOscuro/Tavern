#ifndef COMPONENT_YAML_CONVERSIONS_HPP
#define COMPONENT_YAML_CONVERSIONS_HPP

#include <cstring>

#include <ryml.hpp>

#include <ecs/entity/type.hpp>

#include "component.h"

#define TAVERN_TAG_DIRECTIVE "!t!"

namespace tavern::component {

// Temporary fix!!
// ryml doesn't support string_views which are not null terminated at the expected position
// and using constexpr with char* may be causing memory leaks
template <typename T>
std::string get_type_name() {
    return std::string(ecs::internal::get_type_name<T>());
}

template <typename T>
std::string get_type_tag()
{
    std::string str = get_type_name<T>();
    str.reserve(str.length() + strlen(TAVERN_TAG_DIRECTIVE));
    str.append(TAVERN_TAG_DIRECTIVE);
    return str;
}

//template <typename T>
//constexpr const char* get_type_tag() {
//    const char* type_name = ecs::internal::get_type_name<T>();
//
//    const size_t directive_len = strlen(TAVERN_TAG_DIRECTIVE);
//    const size_t name_len = strlen(type_name);
//
//    char* tag = new char[directive_len + strlen(type_name) + 1];
//
//    // throws build error if using strncpy here
//    memcpy(tag, TAVERN_TAG_DIRECTIVE, directive_len);
//    memcpy(tag + directive_len, type_name, name_len);
//    tag[directive_len + name_len] = '\0';
//
//    return tag;
//}

bool read(const ryml::ConstNodeRef& n, camera* val);
void write(ryml::NodeRef* n, const camera& val);

bool read(const ryml::ConstNodeRef& n, transform* val);
void write(ryml::NodeRef* n, const transform& val);

bool read(const ryml::ConstNodeRef& n, render_mesh* val);
void write(ryml::NodeRef* n, const render_mesh& val);

} /* end of namespace tavern::component */

namespace glm {

bool read(const ryml::ConstNodeRef& n, mat4* val);
void write(ryml::NodeRef* n, const mat4& val);

bool read(const ryml::ConstNodeRef& n, vec4* val);
void write(ryml::NodeRef* n, const vec4& val);

bool read(const ryml::ConstNodeRef& n, vec3* val);
void write(ryml::NodeRef* n, const vec3& val);

} /* end of namespace glm */

#endif /* end of define guard COMPONENT_YAML_CONVERSIONS_HPP */
