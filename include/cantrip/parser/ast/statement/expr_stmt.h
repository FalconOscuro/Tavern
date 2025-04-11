#ifndef EXPR_STMT_H
#define EXPR_STMT_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

struct expr_stmt : public statement
{
    expression* expr = nullptr;

    expr_stmt(expression* expr) : expr(expr)
    {}

    ~expr_stmt() {
        delete expr;
    }

    void accept(visitor* v) override {
        v->visit_expr_stmt(this);
    }
}; /* class expr_stmt */

} /* namespace cantrip::ast */

#endif /* end of include guard: EXPR_STMT_H */
