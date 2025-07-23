#include "cantrip/ast/statement/var_declare.h"

#include "cantrip/ast/statement/struct.h"

namespace cantrip::ast {

var_declare::var_declare(const char* p_type, const char* p_name):
    vtype(p_type), name(p_name)
{}

var_declare::var_declare(type_info t_info, const char* p_name):
    vtype(t_info), name(p_name)
{}

} /* namespace cantrip::ast */
