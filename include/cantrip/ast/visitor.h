#ifndef AST_VISITOR_H
#define AST_VISITOR_H

namespace cantrip::ast {

class node;

class binary;
class call;
class cast;
class core_type;
class grouping;
class identifier;
class literal;
class unary;

class block;
class component;
class expr_stmt;
class flow;
class for_stmt;
class function;
class if_else;
class return_stmt;
class var_declare;
class while_stmt;

class visitor
{
    friend class binary;
    friend class call;
    friend class cast;
    friend class core_type;
    friend class grouping;
    friend class identifier;
    friend class literal;
    friend class unary;

    friend class block;
    friend class component;
    friend class expr_stmt;
    friend class flow;
    friend class for_stmt;
    friend class function;
    friend class if_else;
    friend class return_stmt;
    friend class var_declare;
    friend class while_stmt;

public:

    void visit(node* n);

protected:

    // expressions
    virtual void visit_binary(binary*)         = 0;
    virtual void visit_call(call*)             = 0;
    virtual void visit_cast(cast*)             = 0;
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
