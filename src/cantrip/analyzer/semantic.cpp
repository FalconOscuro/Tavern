#include "cantrip/analyzer/semantic.h"

#include "cantrip/parser/ast/ast_expr.h"
#include "cantrip/parser/ast/ast_stmt.h"

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

bool environment_stack::check_identifier(const ast::identifier*identifier) const
{
    for (size_t i = 0; i < env_stack.size(); ++i) {
        const environment& env = env_stack[env_stack.size() - (i + 1)];

        if (env.variables.find(identifier->name) != env.variables.end())
            return true;
    }

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

    if (call->caller)
        call->caller->accept(this);

    for (size_t i = 0; i < call->params.size(); ++i)
        call->params[i]->accept(this);
}

void semantic::visit_grouping(ast::grouping* grouping) {
    grouping->expr->accept(this);
}

void semantic::visit_identifier(ast::identifier* identifier)
{
    if (!m_env_stack.check_identifier(identifier))
        throw error::undeclared_identifier(identifier);
}

void semantic::visit_literal(ast::literal* literal) {
    // TODO: Resolve literal type to stack
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

    // first pass, visit var decls for analysis
    for (size_t i = 0; i < component->vars.size(); ++i)
        // Should not cause issues if following visitor pattern
        visit_var_declare(component->vars[i].get());

    // second pass, add declared vars to current scope
    //for (size_t i = 0; i < component->vars.size(); ++i)
        // TODO: Throw err on push failure
    //    m_env_stack.top().push_var(component->vars[i].get());

    // visit functions
    for (size_t i = 0; i < component->funcs.size(); ++i)
        visit_function(component->funcs[i].get());

    m_env_stack.pop();
    m_self_env = nullptr;
}

void semantic::visit_expr_stmt(ast::expr_stmt* expr_stmt) {
    expr_stmt->expr->accept(this);    
}

void semantic::visit_flow(ast::flow* flow) {
    // TODO: Check if scoped inside of while/continue
}

void semantic::visit_for_stmt(ast::for_stmt* for_stmt)
{
    m_env_stack.push();

    for_stmt->loop_expr->accept(this);
    for_stmt->body->accept(this);

    m_env_stack.pop();
}

void semantic::visit_function(ast::function* function)
{
    m_env_stack.push();

    // WARNING: step out of env stack for checking?
    // allows setting default variables as global/member variables
    // maybe disable default params?
    for (size_t i = 0; i < function->params.size(); ++i)
        visit_var_declare(function->params[i].get());

    //for (size_t i = 0; i < function->params.size(); ++i)
    //    m_env_stack.top().push_var(function->params[i].get());

    function->body->accept(this);

    m_env_stack.pop();
}

void semantic::visit_if_else(ast::if_else* if_else)
{
    if_else->condition->accept(this);

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
    // NOTE: Need to be able to check against encapsulated function return type

    if (return_stmt->returned)
        return_stmt->returned->accept(this);
}

void semantic::visit_var_declare(ast::var_declare* var_declare)
{
    if (var_declare->expr)
        var_declare->expr->accept(this);

    if (!m_env_stack.push_var(var_declare))
        throw error::redefinition(var_declare);
}

void semantic::visit_while_stmt(ast::while_stmt* while_stmt)
{
    while_stmt->condition->accept(this);

    // Avoid issues from declaration in single statement loop
    m_env_stack.push();
    while_stmt->exec_stmt->accept(this);
    m_env_stack.pop();
}

} /* namespace cantrip::analyzer */
