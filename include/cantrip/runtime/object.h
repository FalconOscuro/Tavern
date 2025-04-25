#ifndef CANTRIP_OBJECT_H
#define CANTRIP_OBJECT_H

#include <cmath>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "cantrip/ast/statement/type.h"

namespace cantrip::runtime {

typedef int32_t     cantrip_int;
typedef float_t     cantrip_float;
typedef bool        cantrip_bool;
typedef std::string cantrip_string;

class object;
class object_ref;
struct member_info;

// factory?
class object_info
{
public:

    object_info(const ast::type& type);

    inline uint32_t size() const {
        return m_size;
    }

    inline uint8_t align() const {
        return m_align;
    }

    uint32_t offset_of(const std::string_view member) const;

    object_ref create(void* ptr) const;
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

    // TODO: eventually could use shared_ptr to reduce repetition of data?
    object_info info;

    uint32_t offset = 0;
    uint8_t  padding = 0;
}; /* end of struct member_info */

// called object_ref as does not destroy pointed to type when going out of scope
class object_ref
{
public:

    object_ref(const ast::type& type, void* ptr);
    object_ref(const object_info& obj_inf, void* ptr);

    ~object_ref();

    object call(const std::string_view func_name);
    object call(const std::string_view func_name, std::vector<object_ref>& args);

    object_ref get(const std::string_view var_name);

    object_ref operator[](const object_ref& obj);
    object_ref operator[](size_t i);

private:

    void* m_ptr;
}; /* end of class object_ref */

} /* end of namespace cantrip::runtime */

#endif /* end of define guard CANTRIP_OBJECT_H */
