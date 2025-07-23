#ifndef TREE_WALK_INTERPRETER_H
#define TREE_WALK_INTERPRETER_H

#include "cantrip/ast/visitor.h"

namespace cantrip::runtime {

class tree_walk_interpreter : public ast::visitor
{
public:

    tree_walk_interpreter();
    virtual ~tree_walk_interpreter();

    // need to define entry point
    void execute();

protected:

    // expressions
    void visit_binary(ast::binary*) override;
    void visit_call(ast::call*) override;
    void visit_cast(ast::cast*) override;
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

    // stack for maintaining local variables
    // heap un-needed as persistant storage done via ecs components
    // may need some heap for systems?
    // need to track variable stack pos, allocate space on enter func
};

} /* namespace cantrip::runtime */

#endif /* end of include guard: TREE_WALK_INTERPRETER_H */
