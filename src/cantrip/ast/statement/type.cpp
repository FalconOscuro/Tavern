#include "cantrip/ast/statement/type.h"

#include <cassert>
#include <cstring>

#include "cantrip/ast/statement/struct.h"

// move to different file?
#define NULL_TYPE_NAME "null"
#define INT_TYPE_NAME "int"
#define FLOAT_TYPE_NAME "float"
#define STRING_TYPE_NAME "string"
#define BOOL_TYPE_NAME "bool"
#define ENTITY_TYPE_NAME "entity"

namespace cantrip::ast {

type::type():
    m_type(NONE)
{}

type::type(const char* type_name)
{
    if (!type_name || NULL_TYPE_NAME)
        m_type = NONE;

    else if (strcmp(type_name, INT_TYPE_NAME) == 0)
        m_type = CORE_INT;

    else if (strcmp(type_name, FLOAT_TYPE_NAME) == 0)
        m_type = CORE_FLOAT;

    else if (strcmp(type_name, STRING_TYPE_NAME) == 0)
        m_type = CORE_STRING;

    else if (strcmp(type_name, BOOL_TYPE_NAME) == 0)
        m_type = CORE_BOOL;

    else
        set_unresolved_name(type_name);
}

type::type(type_info t_info):
    m_type(t_info)
{
    // throw not assert?
    assert(!is_custom_type_or_unresolved() && "Constructor is for core types only!");
}

type::type(const type& t)
{
    m_type = t.get_type_info();

    if (m_type == UNRESOLVED)
        set_unresolved_name(t.m_data.unresolved_name);

    else if (is_resolved_custom_type())
        m_data.custom = t.m_data.custom;
}

type::~type() {
    clear_data();
}

const std::string_view type::name() const
{
    switch (m_type)
    {
    case NONE:
        return NULL_TYPE_NAME;

    case CORE_INT:
        return INT_TYPE_NAME;

    case CORE_FLOAT:
        return FLOAT_TYPE_NAME;

    case CORE_STRING:
        return STRING_TYPE_NAME;

    case CORE_BOOL:
        return BOOL_TYPE_NAME;

    case CUSTOM_COMPONENT:
    case CUSTOM_CLASS:
    case CUSTOM_STRUCT:
        return m_data.custom->name;

    case UNRESOLVED:
        return m_data.unresolved_name;
    }

    assert(false && "Unexpected fall through!");
    return NULL_TYPE_NAME;
}

c_struct* type::get_custom_type() const {
    assert(is_resolved_custom_type() && "This function is only valid for custom data-types");
    return m_data.custom;
}

void type::resolve(c_struct* type)
{
    assert(m_type == UNRESOLVED && "Type has alread been resolved");
    assert(type->name == m_data.unresolved_name && "Type names must match");

    delete[] m_data.unresolved_name;
    m_data.custom = type;

    switch (type->s_type)
    {
        case COMPONENT:
            m_type = CUSTOM_COMPONENT;
            break;

        case CLASS:
            m_type = CUSTOM_CLASS;
            break;

        case STRUCT:
            m_type = CUSTOM_STRUCT;
            break;
    }
}

type& type::operator=(const type& t)
{
    clear_data();
    m_type = t.get_type_info();

    if (m_type == UNRESOLVED)
        set_unresolved_name(t.m_data.unresolved_name);

    else if (is_resolved_custom_type())
        m_data.custom = t.m_data.custom;

    return *this;
}

bool type::operator==(const type& rhs) const
{
    // both custom fully resolved types
    // allows for fast check against pointers
    // could do additional check against names, but currently deemed un-necessary
    if (m_type == rhs.m_type && is_resolved_custom_type())
        return m_data.custom == rhs.m_data.custom;

    // both custom types with at least one being unresolved
    // first if filters out pairs of resolved types
    else if ((m_type == UNRESOLVED || rhs.m_type == UNRESOLVED)
            && is_custom_type_or_unresolved()
            && rhs.is_custom_type_or_unresolved()
    ) {
        // compare names
        return name() == rhs.name();
    }

    else
        return m_type == rhs.m_type;
}

bool type::operator!=(const type& rhs) const
{
    // see above
    if (m_type == rhs.m_type && is_resolved_custom_type())
        return m_data.custom != rhs.m_data.custom;

    else if ((m_type == UNRESOLVED || rhs.m_type == UNRESOLVED)
            && is_custom_type_or_unresolved()
            && rhs.is_custom_type_or_unresolved()
    ) {
        return name() != rhs.name();
    }

    else
        return m_type != rhs.m_type;
}

bool type::operator==(type_info t) const {
    return m_type == t;
}

bool type::operator!=(type_info t) const {
    return m_type != t;
}

bool type::is_resolved_custom_type() const {
    return m_type == CUSTOM_COMPONENT
        || m_type == CUSTOM_CLASS
        || m_type == CUSTOM_STRUCT
    ;
}

bool type::is_custom_type_or_unresolved() const {
    return is_resolved_custom_type() || m_type == UNRESOLVED;
}

void type::set_unresolved_name(const char* type_name)
{
    assert(type_name != nullptr);

    const size_t type_name_len = strlen(type_name) + 1;
    m_data.unresolved_name = new char[type_name_len];

    memcpy(m_data.unresolved_name, type_name, type_name_len);
    m_type = UNRESOLVED;
}

void type::clear_data() {
    if (m_type == UNRESOLVED)
        delete[] m_data.unresolved_name;
}

} /* namespace cantrip::ast */
