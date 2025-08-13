#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "cantrip/ast/visitor.h"

#include "cantrip/module/module.h"
#include "cantrip/ast/statement/var_declare.h"

#include <deque>
#include <string_view>
#include <unordered_map>

namespace cantrip::analyzer {

// defines the scope for a set of variables/functions
struct environment
{
    // need better way for storing variable types?
    std::unordered_map<std::string_view, const ast::var_declare*> variables;

    bool is_loop = false;
};

struct environment_stack
{
    // check for matching var declares at this level
    bool push_var(const ast::var_declare* var);
    const ast::var_declare* check_identifier(const ast::identifier* identifier) const;

    bool in_loop() const;

    void push(bool is_loop = false) {
        env_stack.emplace_back().is_loop = is_loop;
    }

    void pop() {
        env_stack.pop_back();
    }

    environment& top() {
        return env_stack.back();
    };

    std::deque<environment> env_stack;

    // Store current type on top of stack for expression resolution
};

class semantic final : public ast::visitor
{
public:
    semantic() = default;
    virtual ~semantic() = default;

    void analyze_module(module* module);

protected:

    // expressions
    void visit_binary(ast::binary*) override;
    void visit_call(ast::call*) override;
    void visit_cast(ast::cast*) override;
    void visit_core_type(ast::core_type*) override;
    void visit_grouping(ast::grouping*) override;
    void visit_identifier(ast::identifier*) override;
    void visit_literal(ast::literal*) override;
    void visit_type_check(ast::type_check*) override;
    void visit_unary(ast::unary*) override;

    // statements
    void visit_block(ast::block*) override;
    void visit_component(ast::component*) override;
    void visit_expr_stmt(ast::expr_stmt*) override;
    void visit_flow(ast::flow*) override;
    void visit_for_stmt(ast::for_stmt*) override;
    void visit_function(ast::function*) override;
    void visit_if_else(ast::if_else*) override;
    void visit_return_stmt(ast::return_stmt*) override;
    void visit_system(ast::system*) override;
    void visit_var_declare(ast::var_declare*) override;
    void visit_while_stmt(ast::while_stmt*) override;

private:

    void resolve_func_return_type(ast::function* func);
    bool resolve_type(ast::type& type);

    // todo: proper type conversion checking
    bool is_type_convertible(const ast::type& from, const ast::type& to);

    environment_stack m_env_stack;
    ast::type m_type;

    // currently only used for member function lookup within struct
    // could be useful later on if self/this keyword added
    ast::component* m_self_env;

    // scope for current module being explored, for member function resolution
    module* m_module = nullptr;

    // scope for current function being explored
    // need to change to support systems?
    ast::function* m_function = nullptr;
}; /* end of class semantic_analyzer */

} /* namespace cantrip::analyzer */


#endif /* end of include guard: SEMANTIC_ANALYZER_H */
