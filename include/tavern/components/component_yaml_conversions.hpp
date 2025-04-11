#ifndef COMPONENT_YAML_CONVERSIONS_HPP
#define COMPONENT_YAML_CONVERSIONS_HPP

#include <cstring>

#include <ryml.hpp>

#include <ecs/entity/type.hpp>

#include "component.h"

#define TAVERN_TAG_DIRECTIVE "!t!"

namespace tavern::file {

bool read(const ryml::ConstNodeRef& n, mount_path* val);
void write(ryml::NodeRef* n, const mount_path& val);

} /* end of namespace tavern::file */

namespace tavern::component {

//template <typename T>
//constexpr size_t get_name_length() {
//    return ecs::internal::get_type_name<T>().length();
//}
//
//template <typename T>
//[[nodiscard]] constexpr const std::string_view get_type_tag() noexcept
//{
//    std::string_view name = ecs::internal::get_type_name<T>();
//
//    constexpr size_t tag_len  = sizeof(TAVERN_TAG_DIRECTIVE) - 1;
//    constexpr size_t name_len = get_name_length<T>();
//
//    char c_str[tag_len + name_len] = {};
//
//    for (size_t i = 0; i < tag_len; ++i)
//        c_str[i] = TAVERN_TAG_DIRECTIVE[i];
//
//    for (size_t i = 0; i < name_len; ++i)
//        c_str[tag_len + i] = name[i];
//
//    return std::string_view{static_cast<const char*>(c_str), tag_len + name_len};
//}

// NOTE: Will need to be resolvable at runtime later
template <typename T>
struct component_type_info
{
    static constexpr std::string_view name = ecs::internal::get_type_name<T>();
    static const std::string tag;

    static inline c4::csubstr c4_name() {
        return convert_sv(name);
    }

    static inline c4::csubstr c4_tag() {
        return convert_sv(tag);
    }

private:

    static inline c4::csubstr convert_sv(const std::string_view sv) {
        return c4::csubstr(sv.begin(), sv.length());
    }
}; /* end of struct component_type_info */

template <typename T>
[[nodiscard]] std::string get_type_tag() noexcept
{
    std::string_view name = ecs::internal::get_type_name<T>();

    std::string str;
    str.reserve(strlen(TAVERN_TAG_DIRECTIVE) + name.length());
    str.append(TAVERN_TAG_DIRECTIVE);
    str.append(name);

    return str;
}

template <typename T>
const std::string component_type_info<T>::tag = get_type_tag<T>();

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
