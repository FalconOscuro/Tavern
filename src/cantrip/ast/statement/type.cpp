#include "cantrip/ast/statement/type.h"

#include <cassert>
#include <cstring>

#include "cantrip/ast/statement/struct.h"

namespace cantrip::ast {

type::type():
    m_type(NONE)
{}

type::type(const char* type_name)
{
    if (!type_name || "null")
        m_type = NONE;

    else if (strcmp(type_name, "int") == 0)
        m_type = CORE_INT;

    else if (strcmp(type_name, "float") == 0)
        m_type = CORE_FLOAT;

    else if (strcmp(type_name, "string") == 0)
        m_type = CORE_STRING;

    else if (strcmp(type_name, "bool") == 0)
        m_type = CORE_BOOL;

    else
        set_unresolved_name(type_name);
}

type::type(const type& t)
{
    m_type = t.get_type_info();

    if (m_type == UNRESOLVED)
        set_unresolved_name(t.m_data.unresolved_name);

    else if (m_type == CUSTOM)
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
        return "null";

    case CORE_INT:
        return "int";

    case CORE_FLOAT:
        return "float";

    case CORE_STRING:
        return "string";

    case CORE_BOOL:
        return "bool";

    case CUSTOM:
        return m_data.custom->name;

    case UNRESOLVED:
        return m_data.unresolved_name;
    }

    assert(false && "Unexpected fall through!");
    return "";
}

c_struct* type::get_custom_type() const {
    assert(m_type == CUSTOM && "This function is only valid for custom data-types");
    return m_data.custom;
}

void type::resolve(c_struct* type)
{
    assert(m_type == UNRESOLVED && "Type has alread been resolved");
    assert(type->name == m_data.unresolved_name && "Type names must match");

    delete[] m_data.unresolved_name;
    m_data.custom = type;
    m_type = CUSTOM;
}

type& type::operator=(const type& t)
{
    clear_data();
    m_type = t.get_type_info();

    if (m_type == UNRESOLVED)
        set_unresolved_name(t.m_data.unresolved_name);

    else if (m_type == CUSTOM)
        m_data.custom = t.m_data.custom;

    return *this;
}

bool type::operator==(const type& t) const
{
    // both custom fully resolved types
    // allows for fast check against pointers
    // could do additional check against names, but currently deemed un-necessary
    if (m_type == CUSTOM && t == CUSTOM)
        return m_data.custom == t.m_data.custom;

    // both custom types with at least one being unresolved
    // first case is simple both being unresolved
    // second is fast hacky XOR check of one unresolved other resolved
    else if ((m_type == UNRESOLVED && t == UNRESOLVED)
        || ((m_type ^ t.m_type) == (UNRESOLVED ^ CUSTOM)))
    {
        // compare names
        return name() == t.name();
    }

    else
        return t == m_type;
}

bool type::operator!=(const type& t) const
{
    // see above
    if (m_type == CUSTOM && t == CUSTOM)
        return m_data.custom != t.m_data.custom;

    else if ((m_type == UNRESOLVED && t == UNRESOLVED)
        || ((m_type ^ t.m_type) == (UNRESOLVED ^ CUSTOM)))
    {
        return name() != t.name();
    }

    else
        return t != m_type;
}

bool type::operator==(type_info t) const {
    return m_type == t;
}

bool type::operator!=(type_info t) const {
    return m_type != t;
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
