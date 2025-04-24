#include "cantrip/ast/expression/literal.hpp"

#include <cassert>
#include <cstring>

namespace cantrip::ast {

literal::literal(const file_pos& pos, const char* c):
    expression(pos), m_type(literal_type::STRING)
{
    m_data.string = new char[strlen(c) + 1];
    strcpy(m_data.string, c);
}

int literal::get_integer() const {
    assert(m_type == literal_type::INTEGER);
    return m_data.integer;
}

float literal::get_float() const {
    assert(m_type == literal_type::FLOAT);
    return m_data.floating;
}

const char* literal::get_string() const {
    assert(m_type == literal_type::STRING);
    return m_data.string;
}

} /* namespace cantrip::ast */
