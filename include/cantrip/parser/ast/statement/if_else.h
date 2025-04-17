#ifndef IF_ELSE_H
#define IF_ELSE_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

struct if_else : public statement
{
    if_else() = default;
    ~if_else() = default;

    if_else(const if_else&) = delete;
    void operator=(const if_else&) = delete;

    void accept(visitor* v) override {
        v->visit_if_else(this);
    }

    // if condition evaluation
    u_expression_ptr condition = nullptr;
    // statement processed if condition is truthy
    u_statement_ptr exec_stmt = nullptr;

    // statement processed if condition is falsey
    // a linear linked list of if_else statements acts as such,
    // any other type of statement will be treated as an pure else case
    // set to nullptr for no behaviour on falsey
    u_statement_ptr else_stmt = nullptr;
}; /* class if_else */

} /* namespace cantrip::ast */

#endif /* end of include guard: IF_ELSE_H */
