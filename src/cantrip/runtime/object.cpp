#include "cantrip/runtime/object.h"

#include <cassert>

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
    switch (type.get_type_info().classifier)
    {
        case ast::UNRESOLVED:
        case ast::VOID:
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

        case ast::CORE_ENTITY:
            m_size  = sizeof(cantrip_entity);
            m_align = alignof(cantrip_entity);
            return;

        default:
            break;
    }

    // Custom type handling

    {
        const ast::c_struct* struc = type.get_custom_type();
        auto it = struc->vars_begin();

        // throw error!
        if (it == struc->vars_end())
            return;

        // class must align to first data member
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

            // enforce strictest alignment
            if (m_align < current.info.align())
                m_align = current.info.align();

            m_member_map.emplace(std::make_pair((*it)->name, &current));
        }

        // apply padding for structure alignment when in array
        auto& last = m_members.back();
        uint32_t last_end = last.offset + last.info.size();
        last.padding = calculate_padding(last_end, m_align);

        m_size = last_end + last.padding;
    }

    // array type with more than one value
    if (m_type.array_size > 1)
        m_size *= m_type.array_size;
}

uint32_t object_info::offset_of(const std::string_view member) const
{
    auto found = m_member_map.find(member);

    if (found != m_member_map.end())
        return found->second->offset;

    // throw exception on failure?
    else {
        assert(false && "Failed to find object member");
        return 0;
    }
}

object object_info::create(void* ptr) const
{
    // Handle default values / constructors here :|

    switch (m_type.get_type_info().classifier)
    {
        case ast::CORE_INT:
            return object(this, new(ptr) cantrip_int);

        case ast::CORE_FLOAT:
            return object(this, new(ptr) cantrip_float);

        case ast::CORE_BOOL:
            return object(this, new(ptr) cantrip_bool);

        case ast::CORE_STRING:
            return object(this, new(ptr) cantrip_string);

        case ast::CUSTOM_CLASS:
        case ast::CUSTOM_STRUCT:
        case ast::CUSTOM_COMPONENT:
            for (size_t i = 0; i < m_members.size(); ++i)
                m_members[i].create(ptr);

            return object(this, ptr);

        default:
            throw error::unkown_typename(file_pos(), m_type);
            return object();
    }
}

void object_info::destroy(void* ptr) const
{
    switch (m_type.get_type_info().classifier)
    {
        case ast::CORE_INT:
            delete reinterpret_cast<cantrip_int*>(ptr);
            break;

        case ast::CORE_FLOAT:
            delete reinterpret_cast<cantrip_float*>(ptr);
            break;

        case ast::CORE_BOOL:
            delete reinterpret_cast<cantrip_bool*>(ptr);
            break;

        case ast::CORE_STRING:
            delete reinterpret_cast<cantrip_string*>(ptr);
            break;

        case ast::CUSTOM_CLASS:
        case ast::CUSTOM_STRUCT:
        case ast::CUSTOM_COMPONENT:
            for (size_t i = 0; i < m_members.size(); ++i)
                m_members[i].destroy(ptr);
            break;

        default:
            throw error::unkown_typename(file_pos(), m_type);
            break;
    }
}

object member_info::create(void* ptr) const {
    return info.create(reinterpret_cast<char*>(ptr) + offset);
}

void member_info::destroy(void* ptr) const {
    return info.destroy(reinterpret_cast<char*>(ptr) + offset);
}

} /* end of namespace cantrip::runtime */
