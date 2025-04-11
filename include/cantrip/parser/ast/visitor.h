#ifndef AST_VISITOR_H
#define AST_VISITOR_H

namespace cantrip::ast {

struct node;

struct binary;
struct call;
struct core_type;
struct grouping;
struct identifier;
struct literal;
struct unary;

struct block;
struct component;
struct expr_stmt;
struct flow;
struct for_stmt;
struct function;
struct if_else;
struct return_stmt;
struct var_declare;
struct while_stmt;

class visitor
{
    friend struct binary;
    friend struct call;
    friend struct core_type;
    friend struct grouping;
    friend struct identifier;
    friend struct literal;
    friend struct unary;

    friend struct block;
    friend struct component;
    friend struct expr_stmt;
    friend struct flow;
    friend struct for_stmt;
    friend struct function;
    friend struct if_else;
    friend struct return_stmt;
    friend struct var_declare;
    friend struct while_stmt;

public:

    void visit(node* n);

protected:

    // expressions
    virtual void visit_binary(binary*)         = 0;
    virtual void visit_call(call*)             = 0;
    virtual void visit_core_type(core_type*)   = 0;
    virtual void visit_grouping(grouping*)     = 0;
    virtual void visit_identifier(identifier*) = 0;
    virtual void visit_literal(literal*)       = 0;
    virtual void visit_unary(unary*)           = 0;

    // statements
    virtual void visit_block(block*)             = 0;
    virtual void visit_component(component*)     = 0;
    virtual void visit_expr_stmt(expr_stmt*)     = 0;
    virtual void visit_flow(flow*)               = 0;
    virtual void visit_for_stmt(for_stmt*)       = 0;
    virtual void visit_function(function*)       = 0;
    virtual void visit_if_else(if_else*)         = 0;
    virtual void visit_return_stmt(return_stmt*) = 0;
    virtual void visit_var_declare(var_declare*) = 0;
    virtual void visit_while_stmt(while_stmt*)   = 0;
}; /* class visitor */

} /* namespace cantrip::ast */

#endif /* end of include guard: AST_VISITOR_H */
