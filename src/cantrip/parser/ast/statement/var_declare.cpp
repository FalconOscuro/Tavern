#include "cantrip/parser/ast/statement/var_declare.h"

#include "cantrip/parser/ast/statement/struct.h"

namespace cantrip::ast {

var_declare::var_declare(const char* p_type, const char* p_name):
    vtype(p_type), name(p_name)
{}

} /* namespace cantrip::ast */
