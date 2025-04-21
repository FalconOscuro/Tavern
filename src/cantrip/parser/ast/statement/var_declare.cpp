#include "cantrip/parser/ast/statement/var_declare.h"

#include <cassert>
#include <cstring>

#include "cantrip/parser/ast/statement/struct.h"

namespace cantrip::ast {

var_type::var_type(const char* type_name)
{
    if (strcmp(type_name, "int") == 0)
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

var_type::var_type(const var_type& t)
{
    m_type = t.type_info();

    if (m_type == UNRESOLVED)
        set_unresolved_name(t.m_data.unresolved_name);

    else if (m_type == CUSTOM)
        m_data.custom = t.m_data.custom;
}

var_type::~var_type() {
    clear_data();
}

const std::string_view var_type::name() const
{
    switch (m_type)
    {
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

const c_struct* var_type::get_custom_type() const {
    assert(m_type == CUSTOM && "This function is only valid for custom data-types");
    return m_data.custom;
}

void var_type::resolve(const c_struct* type)
{
    assert(m_type == UNRESOLVED && "Type has alread been resolved");
    assert(type->name == m_data.unresolved_name && "Type names must match");

    delete[] m_data.unresolved_name;
    m_data.custom = type;
    m_type = CUSTOM;
}

var_type& var_type::operator=(const var_type& t)
{
    clear_data();
    m_type = t.type_info();

    if (m_type == UNRESOLVED)
        set_unresolved_name(t.m_data.unresolved_name);

    else if (m_type == CUSTOM)
        m_data.custom = t.m_data.custom;

    return *this;
}

void var_type::set_unresolved_name(const char* type_name)
{
    assert(type_name != nullptr);

    const size_t type_name_len = strlen(type_name) + 1;
    m_data.unresolved_name = new char[type_name_len];

    memcpy(m_data.unresolved_name, type_name, type_name_len);
    m_type = UNRESOLVED;
}

void var_type::clear_data() {
    if (m_type == UNRESOLVED)
        delete[] m_data.unresolved_name;
}

var_declare::var_declare(const char* p_type, const char* p_name):
    type(p_type), name(p_name)
{}

} /* namespace cantrip::ast */
