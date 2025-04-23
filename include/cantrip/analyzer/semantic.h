#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "cantrip/parser/ast/visitor.h"

#include "cantrip/parser/ast/module.h"
#include "cantrip/parser/ast/statement/var_declare.h"

#include <deque>
#include <string_view>
#include <unordered_map>

namespace cantrip::analyzer {

// defines the scope for a set of variables/functions
struct environment
{
    // need better way for storing variable types?
    std::unordered_map<std::string_view, const ast::var_declare*> variables;
};

struct environment_stack
{
    // check for matching var declares at this level
    bool push_var(const ast::var_declare* var);
    bool check_identifier(const ast::identifier* identifier) const;

    void push() {
        env_stack.emplace_back();
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

class semantic : public ast::visitor
{
public:
    semantic();
    virtual ~semantic();

    void analyze_module(ast::module* module);

protected:

    // expressions
    void visit_binary(ast::binary*) override;
    void visit_call(ast::call*) override;
    void visit_core_type(ast::core_type*) override;
    void visit_grouping(ast::grouping*) override;
    void visit_identifier(ast::identifier*) override;
    void visit_literal(ast::literal*) override;
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
    void visit_var_declare(ast::var_declare*) override;
    void visit_while_stmt(ast::while_stmt*) override;

private:

    environment_stack m_env_stack;
    ast::var_type m_type;

    // currently only used for member function lookup within struct
    // could be useful later on if self/this keyword added
    const ast::component* m_self_env;

    ast::module* m_module;
}; /* end of class semantic_analyzer */

} /* namespace cantrip::analyzer */


#endif /* end of include guard: SEMANTIC_ANALYZER_H */
