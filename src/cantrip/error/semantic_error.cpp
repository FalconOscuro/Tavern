#include "cantrip/error/semantic_error.h"

#include <string>

namespace cantrip::error {

redefinition::redefinition(const ast::c_struct* struc):
    exception(struc->pos, std::string("Redefinition of custom data structure '").append(struc->name).append("'").c_str())
{}

redefinition::redefinition(const ast::function* func):
    exception(func->pos, std::string("Redefinition of function '").append(func->name).append("'").c_str())
{}

redefinition::redefinition(const ast::system* sys):
    exception(sys->pos, std::string("Redefinition of system '").append(sys->name).append("'").c_str())
{}

redefinition::redefinition(const ast::var_declare* var):
    exception(var->pos, std::string("Redefinition of variable '").append(var->name).append("'").c_str())
{}

undeclared_identifier::undeclared_identifier(const ast::identifier* ident):
    exception(ident->pos, std::string("Undeclared identifier '").append(ident->name).append("'").c_str())
{}

unkown_typename::unkown_typename(const ast::function* func):
    unkown_typename(func->pos, func->return_type)
{}

unkown_typename::unkown_typename(const ast::var_declare* var):
    unkown_typename(var->pos, var->vtype)
{}

unkown_typename::unkown_typename(const ast::call* call):
    exception(call->pos, std::string("Could not find a constructor for undeclared type '").append(call->name).append("'").c_str())
{}

unkown_typename::unkown_typename(const ast::cast* cast):
    unkown_typename(cast->pos, cast->as_type)
{}

unkown_typename::unkown_typename(const ast::type_check* type_check):
    unkown_typename(type_check->pos, type_check->is_type)
{}

unkown_typename::unkown_typename(const file_pos& pos, const ast::type& type):
    exception(pos, std::string("Undeclared identifier '").append(type.name()).append("'").c_str())
{}

not_in_loop::not_in_loop(const ast::flow* flow):
    exception(flow->pos, std::string("Flow statements can only be used within a loop").c_str())
{}

not_structure::not_structure(const file_pos& pos, const ast::type& type):
    exception(pos, std::string("Core type '").append(type.name()).append("' has no callable members").c_str())
{}

no_member::no_member(const ast::call* call, const ast::c_struct* struc):
    exception(call->pos, std::string("'").append(call->name).append("' is not a member of data-type '").append(struc->name).append("'").c_str())
{}

type_not_convertible::type_not_convertible(const file_pos& pos, const ast::type& from, const ast::type& to):
    exception(pos, std::string("Type '").append(from.name()).append(std::string("' is not convertible to '").append(to.name()).append("'")).c_str())
{}

invalid_system::invalid_system(const ast::system* sys):
    exception(sys->pos, std::string("System '").append(sys->name).append("' must declare at least one(1) valid component as a parameter").c_str())
{}

invalid_system::invalid_system(const ast::system* sys, const ast::var_declare* var):
    exception(var->pos, std::string("Invalid parameter '").append(var->name).append("' for system '").append(sys->name).append("', all system parameter declarations must be valid component types").c_str())
{}

} /* namespace cantrip::error */
