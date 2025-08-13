#ifndef CANTRIP_OBJECT_H
#define CANTRIP_OBJECT_H

#include <cmath>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "cantrip/ast/statement/type.h"
#include "ecs/entity/entity.h"

namespace cantrip::runtime {

typedef int32_t          cantrip_int;
typedef float_t          cantrip_float;
typedef bool             cantrip_bool;
typedef std::string      cantrip_string;
typedef ecs::entity_type cantrip_entity;

class object;
struct member_info;

// factory?
class object_info
{
public:

    object_info(const ast::type& type);
    ~object_info() = default;

    inline uint32_t size() const {
        return m_size;
    }

    inline uint8_t align() const {
        return m_align;
    }

    uint32_t offset_of(const std::string_view member) const;

    object create(void* ptr) const;

    void destroy(object& obj) const;
    void destroy(void* ptr) const;

    inline const ast::type& get_type() const {
        return m_type;
    }

private:

    ast::type m_type;

    std::vector<member_info> m_members;
    std::unordered_map<std::string_view, const member_info*> m_member_map;

    uint32_t m_size;
    uint8_t m_align;
}; /* end of class object_info */

struct member_info
{
    template <class... Args>
    member_info(Args&&... args):
        info(args...)
    {}

    object create(void* ptr) const;
    void destroy(void* ptr) const;

    // TODO: eventually could use shared_ptr to reduce repetition of data?
    object_info info;

    uint32_t offset = 0;
    uint8_t  padding = 0;
}; /* end of struct member_info */

// called object_ref as does not destroy pointed to type when going out of scope
class object
{
public:

    object();
    object(const object_info* obj_inf, void* ptr);

    ~object();

    bool is_ref() const {
        return m_is_ref;
    }

    object call(const std::string_view func_name);
    object call(const std::string_view func_name, std::vector<object>& args);

    object get(const std::string_view var_name);

    cantrip_int*    get_int();
    cantrip_float*  get_float();
    cantrip_bool*   get_bool();
    cantrip_string* get_string();
    cantrip_entity* get_entity();

    const cantrip_int*    get_int() const;
    const cantrip_float*  get_float() const;
    const cantrip_bool*   get_bool() const;
    const cantrip_string* get_string() const;
    const cantrip_entity* get_entity() const;

    object operator[](const object& obj);
    object operator[](size_t i);

    const ast::type get_type() const;

private:

    void* m_ptr;

    // WARNING: Need to ensure all instances of object are not outlived
    // by their descriptor, enforce this in the factory
    object_info* m_info;

    bool m_is_ref;
}; /* end of class object_ref */

} /* end of namespace cantrip::runtime */

#endif /* end of define guard CANTRIP_OBJECT_H */
