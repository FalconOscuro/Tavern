#include "cantrip/ast/expression/identifier.h"

#include <cstring>

#include "cantrip/error/exception.h"

namespace cantrip::ast {

identifier::identifier(const file_pos& pos, const char* s):
    expression(pos)
{
    m_name_len = strlen(s);

    if (m_name_len == 0)
        throw error::exception(pos, "Identifier recieved name with len 0!");

    m_name = new char[m_name_len + 1];
    memcpy(m_name, s, m_name_len + 1);
}

identifier::~identifier() {
    destroy_name_buffer();
}

void identifier::set_var_info(const var_declare* var_info)
{
    // post warning?
    if (resolved() || var_info == nullptr)
        return;

    else if (name() != var_info->name)
            throw error::exception(pos, "identifier name mismatch during attempted identifier resolution!"); // could be more informative w/ names?

    destroy_name_buffer();
    m_var_info = var_info;
}

const std::string_view identifier::name() const
{
    return resolved() ? m_var_info->name : std::string_view(m_name, m_name_len);
}

void identifier::destroy_name_buffer()
{
    if (!resolved())
    {
        delete [] m_name;
        m_name = nullptr;
        m_name_len = 0;
    }
}

} /* namespace cantrip::ast */
