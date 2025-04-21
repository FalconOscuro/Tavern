#include "cantrip/error/semantic_error.h"

namespace cantrip::error {

redefinition::redefinition(const ast::var_declare* var):
    exception(var->pos, std::string("Redefinition of '").append(var->name).append("'").c_str())
{}

} /* namespace cantrip::error */
