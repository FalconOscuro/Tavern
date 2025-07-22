#include "cantrip/error/semantic_error.h"

#include <string>

namespace cantrip::error {

redefinition::redefinition(const ast::c_struct* struc):
    exception(struc->pos, std::string("Redefinition of custom data structure '").append(struc->name).append("'").c_str())
{}

redefinition::redefinition(const ast::function* func):
    exception(func->pos, std::string("Redefinition of function '").append(func->name).append("'").c_str())
{}

redefinition::redefinition(const ast::var_declare* var):
    exception(var->pos, std::string("Redefinition of variable '").append(var->name).append("'").c_str())
{}

undeclared_identifier::undeclared_identifier(const ast::identifier* ident):
    exception(ident->pos, std::string("Undeclared identifier '").append(ident->name).append("'").c_str())
{}

unkown_typename::unkown_typename(const ast::var_declare* var):
    unkown_typename(var->pos, var->vtype)
{}

unkown_typename::unkown_typename(const file_pos& pos, const ast::type& type):
    exception(pos, std::string("Undeclared identifier '").append(type.name()).append("'").c_str())
{}

type_not_convertible::type_not_convertible(const file_pos& pos, const ast::type& from, const ast::type& to):
    exception(pos, std::string("Type '").append(from.name()).append(std::string("' is not convertible to '").append(to.name()).append("'")).c_str())
{}

} /* namespace cantrip::error */
