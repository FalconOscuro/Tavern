#include "cantrip/analyzer/semantic.h"

#include "cantrip/ast/ast_expr.h"
#include "cantrip/ast/ast_stmt.h"

#include "cantrip/error/semantic_error.h"

namespace cantrip::analyzer {

bool environment_stack::push_var(const ast::var_declare* var)
{
    // always ensure stack is populated
    if (env_stack.empty())
        push();

    environment& env = top();

    const std::string_view name = var->name;

    // redefinition
    if (env.variables.find(name) != env.variables.end())
        return false;

    else {
        env.variables.emplace(std::make_pair(name, var));
        return true;
    }
}

const ast::var_declare* environment_stack::check_identifier(const ast::identifier*identifier) const
{
    for (size_t i = 0; i < env_stack.size(); ++i) {
        const environment& env = env_stack[env_stack.size() - (i + 1)];
        auto found = env.variables.find(identifier->name);

        if (found != env.variables.end())
            return found->second;
    }

    return nullptr;
}

bool environment_stack::in_loop() const
{
    for (size_t i = 0; i < env_stack.size(); ++i)
        if (env_stack[i].is_loop)
            return true;

    return false;
}

void semantic::analyze_module(ast::module* module)
{
    m_module = module;

    // iterate components
    for (auto it = module->components.begin(); it != module->components.end(); ++it)
        visit_component(it->second.get());

    // iterate functions
    for (auto it = module->functions.begin(); it != module->functions.end(); ++it)
        visit_function(it->second.get());

    m_module = nullptr;
}

// expressions
void semantic::visit_binary(ast::binary* binary)
{
    // TODO: need to resolve type of operands and check compatibility
    binary->left->accept(this);
    binary->right->accept(this);
}

void semantic::visit_call(ast::call* call)
{
    // TODO: Resolve types of args and check function exists
    ast::function* func = nullptr;

    if (call->caller)
    {
        call->caller->accept(this);

        if (m_type == ast::UNRESOLVED || m_type == ast::NONE)
            throw error::unkown_typename(call->caller->pos, m_type);

        else if (m_type == ast::CUSTOM)
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

    if (!resolve_type(cast->as_type) || cast->as_type == ast::NONE)
        throw error::unkown_typename(cast);

    // check if type is convertable...
    m_type = cast->as_type;
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
        m_type = var->vtype;
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

void semantic::visit_unary(ast::unary* unary)
{
    // TODO: check type is valid
    unary->expr->accept(this);
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

void semantic::visit_component(ast::component* component)
{
    m_self_env = component;

    m_env_stack.push();

    // visit vars
    for (auto it = component->vars_begin(); it != component->vars_end(); ++it)
        // Should not cause issues if following visitor pattern
        visit_var_declare(*it);

    // visit functions
    for (auto it = component->funcs_begin(); it != component->funcs_end(); ++it)
        visit_function(*it);

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

    for_stmt->body->accept(this);

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
    // not in function throw error, should be impossible?
    if (!m_function);

    if (return_stmt->returned)
        return_stmt->returned->accept(this);

    // mismatched return type, throw error
    if (m_type != m_function->return_type);

    m_type = ast::type();
}

void semantic::visit_var_declare(ast::var_declare* var_declare)
{
    if (!resolve_type(var_declare->vtype) || var_declare->vtype == ast::NONE) {
        throw error::unkown_typename(var_declare);
    }

    if (var_declare->expr)
    {
        var_declare->expr->accept(this);

        // type mismatch throw err
        if (m_type != var_declare->vtype);

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

void semantic::resolve_func_return_type(ast::function* func) {
    if (!resolve_type(func->return_type))
        throw error::unkown_typename(func);
}

bool semantic::resolve_type(ast::type& type)
{
    if (type != ast::UNRESOLVED)
        return true;

    auto found = m_module->components.find(type.name());

    if (found == m_module->components.end())
        return false;

    type.resolve(found->second.get());
    return true;
}

} /* namespace cantrip::analyzer */
