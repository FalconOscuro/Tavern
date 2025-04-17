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

type::type(token_type t)
{
    if (t < token_type::CORE_TYPE_START || t > token_type::CORE_TYPE_END)
        throw std::invalid_argument("Construction of type statement from token, must be of a core type!");

    info = (tinfo)(t - token_type::CORE_TYPE_START);
}

} /* end of namespace cantrip::ast */
