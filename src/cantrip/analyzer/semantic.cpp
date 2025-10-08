#include "cantrip/analyzer/semantic.h"

#include "cantrip/ast/ast_expr.h"
#include "cantrip/ast/ast_stmt.h"

#include "cantrip/ast/statement/type.h"
#include "cantrip/error/semantic_error.h"

namespace cantrip::analyzer {

void semantic::analyze_module(module* module)
{
    m_module = module;

    // iterate components
    for (auto it = module->components.begin(); it != module->components.end(); ++it)
        visit_component(it->second.get());

    // iterate functions
    for (auto it = module->functions.begin(); it != module->functions.end(); ++it)
        visit_function(it->second.get());

    // iterate systems
    for (auto it = module->systems.begin(); it != module->systems.end(); ++it)
        visit_system(it->second.get());

    m_module = nullptr;
}

// expressions
void semantic::visit_binary(ast::binary* binary)
{
    binary->left->accept(this);

    ast::type type_l = m_type;

    // hacky, but if attribute, right depends on scope of left
    if (binary->type == ast::ATTRIBUTE)
    {
        m_env_stack.push();

        if (type_l.is_resolved_custom_type())
        {
            const ast::c_struct* l_struct = type_l.get_custom_type();

            for (auto it = l_struct->vars_begin(); it != l_struct->vars_end(); ++it)
                m_env_stack.push_var(it->get());
        }

        else
            throw error::exception(binary->left->pos, "Core types do not have attributes");

        binary->right->accept(this);
        
        m_env_stack.pop();
    }

    else
        binary->right->accept(this);

    static const ast::type TYPE_BOOL = ast::type(TYPE_BOOLEAN);

    switch (binary->type)
    {
    case ast::IS_EQUAL:
    case ast::NOT_EQUAL:
        if (type_l != m_type)
            throw error::type_not_convertible(binary->pos, type_l, m_type);

        else if (type_l.array_size > 0 || m_type.array_size > 0)
            throw error::exception(binary->pos, std::string("Cannot compare arrays").c_str());

        m_type = TYPE_BOOL;
        break;

    case ast::LOGICAL_AND:
    case ast::LOGICAL_OR:

        if (!is_type_convertible(type_l, TYPE_BOOL) || type_l.array_size > 0)
            throw error::type_not_convertible(binary->pos, type_l, TYPE_BOOL);

        if (!is_type_convertible(m_type, TYPE_BOOL) || m_type.array_size > 0)
            throw error::type_not_convertible(binary->pos, m_type, TYPE_BOOL);

        m_type = TYPE_BOOL;
        break;

    case ast::BITWISE_OR:
    case ast::BITWISE_AND:
        throw error::exception(binary->pos, "Bitwise operators are unsupported");
        break;

    // only support numericals currently
    case ast::LESS_THAN:
    case ast::GREATER_THAN:
    case ast::LESS_THAN_EQUAL:
    case ast::GREATER_THAN_EQUAL:
        if (type_l != TYPE_INTEGER && type_l != TYPE_FLOAT)
            throw error::exception(binary->pos, "Arithmetic operators currently only support numerical (int or float) types!");

        if (!is_type_convertible(m_type, type_l))
            throw error::type_not_convertible(binary->pos, m_type, type_l);

        m_type = TYPE_BOOL;
        break;

    case ast::ADD:
    case ast::SUBTRACT:
    case ast::MULTIPLY:
    case ast::DIVIDE:
    case ast::ASSIGN_ADD:
    case ast::ASSIGN_SUBTRACT:
    case ast::ASSIGN_DIVIDE:
    case ast::ASSIGN_MULTIPLY:

        if (type_l != TYPE_INTEGER && type_l != TYPE_FLOAT)
            throw error::exception(binary->pos, "Arithmetic operators currently only support numerical (int or float) types!");

    // fallthrough
    case ast::ASSIGN:
        if (!is_type_convertible(m_type, type_l))
            throw error::type_not_convertible(binary->pos, m_type, type_l);

        m_type = type_l;
        break;

    case ast::SUBSCRIPTION:

        if (type_l.array_size == 0)
            throw error::exception(binary->left->pos, "Subscription can only be used on array types!");

        if (m_type != TYPE_INTEGER)
            throw error::type_not_convertible(binary->right->pos, m_type, ast::type(TYPE_INTEGER));

        m_type = type_l;
        m_type.array_size = 0;
        break;

    case ast::ATTRIBUTE:
        break;
    }
}

void semantic::visit_call(ast::call* call)
{
    // TODO: Resolve types of args and check function exists
    ast::function* func = nullptr;

    if (call->caller)
    {
        call->caller->accept(this);

        if (m_type == ast::UNRESOLVED || m_type == ast::VOID)
            throw error::unkown_typename(call->caller->pos, m_type);

        else if (m_type.is_resolved_custom_type())
        {
            ast::c_struct* struc = m_type.get_custom_type();

            func = struc->try_get_func(call->name);

            if (func == nullptr)
                throw error::no_member(call, struc);
        }

        else
            throw error::not_structure(call->caller->pos, m_type);
    }

    else
    {
        // check member scope
        if (m_self_env != nullptr)
            func = m_self_env->try_get_func(call->name);

        // not in member scope
        if (func == nullptr)
        {
            auto found = m_module->functions.find(call->name);

            // TODO: resolve as constructor!
            if (found == m_module->functions.end())
                throw error::unkown_typename(call);

            func = found->second.get();
        }
    }

    resolve_func_return_type(func);
    m_type = func->return_type;

    for (size_t i = 0; i < call->params.size(); ++i)
        call->params[i]->accept(this);
}

void semantic::visit_cast(ast::cast* cast)
{
    cast->expr->accept(this);

    if (!resolve_type(cast->as_type) || cast->as_type == ast::VOID)
        throw error::unkown_typename(cast);

    if (!is_type_convertible(m_type, cast->as_type))
        throw error::type_not_convertible(cast->pos, m_type, cast->as_type);

    m_type = cast->as_type;
}

void semantic::visit_core_type(ast::core_type* core_type)
{
    // UNIMPLEMENTED
    (void)core_type;
    // set m_type?
}

void semantic::visit_grouping(ast::grouping* grouping) {
    grouping->expr->accept(this);
}

void semantic::visit_identifier(ast::identifier* identifier)
{
    const ast::var_declare* var = m_env_stack.check_identifier(identifier);

    if (var == nullptr)
        throw error::undeclared_identifier(identifier);

    else
    {
        m_type = var->vtype;
        identifier->set_var_info(var);
    }
}

void semantic::visit_literal(ast::literal* literal)
{
    switch (literal->get_type())
    {
    case ast::INTEGER:
        m_type = ast::type(ast::CORE_INT);
        break;

    case ast::FLOAT:
        m_type = ast::CORE_FLOAT;
        break;

    case ast::TRUE:
    case ast::FALSE:
        m_type = ast::type(ast::CORE_BOOL);
        break;

    case ast::STRING:
        m_type = ast::type(ast::CORE_STRING);
        break;

    default:
        m_type = ast::type();
    }
}

void semantic::visit_type_check(ast::type_check* type_check)
{
    // could make this compile constant
    // currently un-neccessary until upcasting exists, as this isn't a dynamic language...
    // can use for checking if entities have components?
    type_check->expr->accept(this);

    if (!resolve_type(type_check->is_type) || type_check->is_type == ast::VOID)
        throw error::unkown_typename(type_check);

    // constant optimization with bool literal?
    m_type = ast::type(ast::CORE_BOOL);
}

void semantic::visit_unary(ast::unary* unary)
{
    unary->expr->accept(this);

    static const ast::type TYPE_BOOL = ast::type(TYPE_BOOLEAN);

    switch (unary->type)
    {
    case ast::LOGICAL_NOT:
        if (!is_type_convertible(m_type, TYPE_BOOL))
            throw error::type_not_convertible(unary->pos, m_type, TYPE_BOOL);

        m_type = TYPE_BOOL;
        break;

    case ast::MINUS:
        if (m_type != TYPE_INTEGER && m_type != TYPE_FLOAT)
            throw error::exception(unary->pos, "Unary minus only supports numerical types (int, float)");
        break;
    }
}

// statements
void semantic::visit_block(ast::block* block)
{
    m_env_stack.push();

    for (size_t i = 0; i < block->stmts.size(); ++i) {
        block->stmts[i]->accept(this);
    }

    m_env_stack.pop();
}

// should be checking all component vars before moving to component funcs
void semantic::visit_component(ast::component* component)
{
    m_self_env = component;

    m_env_stack.push();

    // visit vars
    for (auto it = component->vars_begin(); it != component->vars_end(); ++it)
        // Should not cause issues if following visitor pattern
        visit_var_declare(it->get());

    // visit functions
    for (auto it = component->funcs_begin(); it != component->funcs_end(); ++it)
        visit_function(it->get());

    m_env_stack.pop();
    m_self_env = nullptr;
}

void semantic::visit_expr_stmt(ast::expr_stmt* expr_stmt) {
    expr_stmt->expr->accept(this);    
    // discard type info
    m_type = ast::type();
}

void semantic::visit_flow(ast::flow* flow)
{
    if (!m_env_stack.in_loop())
        throw error::not_in_loop(flow);
}

void semantic::visit_for_stmt(ast::for_stmt* for_stmt)
{
    m_env_stack.push(true);

    // UNIMPLEMENTED && TEMPORARY!!
    for_stmt->loop_expr->accept(this);
    m_type = ast::type();

    for_stmt->exec_stmt->accept(this);

    m_env_stack.pop();
}

void semantic::visit_function(ast::function* function)
{
    resolve_func_return_type(function);

    // Check if already in function? should be impossible??
    m_function = function;
    m_env_stack.push();

    // WARNING: step out of env stack for checking?
    // allows setting default variables as global/member variables
    // maybe disable default params?
    for (size_t i = 0; i < function->params.size(); ++i)
        visit_var_declare(function->params[i].get());

    // need to check return type, requires caching state as being in function to compare return type in flow

    function->body->accept(this);

    m_env_stack.pop();
    m_function = nullptr;
}

void semantic::visit_if_else(ast::if_else* if_else)
{
    if_else->condition->accept(this);
    // check boolean?
    m_type = ast::type();

    // prevent case where single statement if/else
    // defines variable, should warn user if never used
    m_env_stack.push();
    if_else->exec_stmt->accept(this);
    m_env_stack.pop();

    if (if_else->else_stmt)
        if_else->else_stmt->accept(this);
}

void semantic::visit_return_stmt(ast::return_stmt* return_stmt)
{
    // need way to check if in system func
    if (!m_function)
    {
        return;
    }

    if (return_stmt->returned)
        return_stmt->returned->accept(this);

    // mismatched return type, throw error
    if (!is_type_convertible(m_type, m_function->return_type))
        throw error::type_not_convertible(return_stmt->pos, m_type, m_function->return_type);

    m_type = ast::type();
}

void semantic::visit_system(ast::system* sys)
{
    m_env_stack.push();
    // TODO: Add default eid param

    if (sys->params.empty())
        throw error::invalid_system(sys);

    // check if component occurs multiple times?
    for (size_t i = 0; i < sys->params.size(); ++i)
    {
        visit_var_declare(sys->params[i].get());

        if (sys->params[i]->vtype != ast::CUSTOM_COMPONENT)
            throw error::invalid_system(sys, sys->params[i].get());
    }

    sys->body->accept(this);

    m_env_stack.pop();
}

void semantic::visit_var_declare(ast::var_declare* var_declare)
{
    if (!resolve_type(var_declare->vtype) || var_declare->vtype == ast::VOID) {
        throw error::unkown_typename(var_declare);
    }

    if (var_declare->expr)
    {
        var_declare->expr->accept(this);

        // type mismatch throw err
        if (!is_type_convertible(m_type, var_declare->vtype))
            throw error::type_not_convertible(var_declare->pos, m_type, var_declare->vtype);

        m_type = ast::type();
    }

    if (!m_env_stack.push_var(var_declare))
        throw error::redefinition(var_declare);
}

void semantic::visit_while_stmt(ast::while_stmt* while_stmt)
{
    while_stmt->condition->accept(this);
    m_type = ast::type();

    // Avoid issues from declaration in single statement loop
    m_env_stack.push(true);
    while_stmt->exec_stmt->accept(this);
    m_env_stack.pop();
}

void semantic::resolve_struct_vars(ast::c_struct* c_struct)
{
    unsigned long int offset = 0;

    for (auto it = c_struct->vars_begin(); it != c_struct->vars_end(); ++it)
    {
        if (!resolve_type(it->get()->vtype) || it->get()->vtype == ast::VOID)
            throw error::unkown_typename(it->get());

        // check for circular dependencies
        // get type size
    }
}

void semantic::resolve_func_return_type(ast::function* func) {
    if (!resolve_type(func->return_type))
        throw error::unkown_typename(func);
}

bool semantic::resolve_type(ast::type& type)
{
    if (type != ast::UNRESOLVED)
        return true;

    auto found = m_module->components.find(type.name().data());

    if (found == m_module->components.end())
        return false;

    type.resolve(found->second.get());
    return true;
}

// could make throw exception?
bool semantic::is_type_convertible(const ast::type& from, const ast::type& to)
{
    // simple implementation only checks if same
    return from == to;
}

} /* namespace cantrip::analyzer */
