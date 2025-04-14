#include "cantrip/parser/ast/statement/type.h"

#include <cstddef>
#include <cstring>
#include <stdexcept>

namespace cantrip::ast {

const char* const CORE_TYPE_NAMES[] = {
    "int"   ,
    "bool"  ,
    "float" ,
    "string"
};

type::type(token::type t)
{
    if (t < token::CORE_TYPE_START || t > token::CORE_TYPE_END)
        throw std::invalid_argument("Construction of type statement from token, must be of a core type!");

    info = (tinfo)(t - token::CORE_TYPE_START);

    set_name(CORE_TYPE_NAMES[info]);
}

type::type(const char* s)
{
    // compare str incase passing core_type
    for (size_t i = 0; i < sizeof(CORE_TYPE_NAMES) / sizeof(char*); ++i) {
        if (strcmp(s, CORE_TYPE_NAMES[i]) == 0) {
            info = (tinfo)i;
            break;
        }
    }
    
    set_name(s);
}

type::~type() {
    delete[] name;
}

void type::set_name(const char* c)
{
    if (name != nullptr)
        delete[] name;

    name = new char[strlen(c) + 1];
    strcpy(name, c);
}

} /* end of namespace cantrip::ast */
