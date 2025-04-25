#include "cantrip/runtime/object.h"

#include "cantrip/error/semantic_error.h"
#include "cantrip/ast/statement/struct.h"
#include "cantrip/ast/statement/var_declare.h"

namespace cantrip::runtime {

inline uint8_t calculate_padding(uint32_t offset, uint8_t align)
{
    const uint8_t modulo = offset % align;
    return modulo ? align - modulo : 0;
}

object_info::object_info(const ast::type& type):
    m_type(type)
{
    switch (type.get_type_info())
    {
    case ast::UNRESOLVED:
    case ast::NONE:
        throw error::unkown_typename(file_pos(), type);
        return;

    case ast::CORE_INT:
        m_size  = sizeof(cantrip_int);
        m_align = alignof(cantrip_int);
        return;

    case ast::CORE_FLOAT:
        m_size  = sizeof(cantrip_float);
        m_align = alignof(cantrip_float);
        return;

    case ast::CORE_BOOL:
        m_size  = sizeof(cantrip_bool);
        m_align = alignof(cantrip_bool);
        return;

    case ast::CORE_STRING:
        m_size  = sizeof(cantrip_string);
        m_align = alignof(cantrip_string);
        return;

    case ast::CUSTOM:
        break;
    }

    // Custom type handling

    {
        const ast::c_struct* struc = type.get_custom_type();
        auto it = struc->vars_begin();

        // throw error!
        if (it == struc->vars_end())
            return;

        auto& first = m_members.emplace_back((*it)->vtype);
        m_align = first.info.align();
    
        m_member_map.emplace(std::make_pair((*it)->name, &first));

        for (++it; it != struc->vars_end(); ++it)
        {
            auto& previous = m_members.back();
            auto& current  = m_members.emplace_back((*it)->vtype);

            uint32_t previous_end = previous.offset + previous.info.size();
            previous.padding = calculate_padding(previous_end, current.info.align());

            current.offset = previous_end + previous.padding;

            m_member_map.emplace(std::make_pair((*it)->name, &current));
        }

        // apply padding for structure alignment when in array
        auto& last = m_members.back();
        uint32_t last_end = last.offset + last.info.size();
        last.padding = calculate_padding(last_end, m_align);

        m_size = last_end + last.padding;
    }
}

} /* end of namespace cantrip::runtime */
