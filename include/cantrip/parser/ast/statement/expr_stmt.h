#ifndef EXPR_STMT_H
#define EXPR_STMT_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

struct expr_stmt : public statement
{
    expr_stmt(expression* expr) : expr(expr) {}
    ~expr_stmt() = default;

    expr_stmt(const expr_stmt&) = delete;
    void operator=(const expr_stmt&) = delete;

    void accept(visitor* v) override {
        v->visit_expr_stmt(this);
    }

    u_expression_ptr expr = nullptr;
}; /* class expr_stmt */

} /* namespace cantrip::ast */

#endif /* end of include guard: EXPR_STMT_H */
