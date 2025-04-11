#ifndef IF_ELSE_H
#define IF_ELSE_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

struct if_else : public statement
{
    // if condition evaluation
    expression* condition = nullptr;
    // statement processed if condition is truthy
    statement* exec_stmt = nullptr;

    // statement processed if condition is falsey
    // a linear linked list of if_else statements acts as such,
    // any other type of statement will be treated as an pure else case
    // set to nullptr for no behaviour on falsey
    statement* else_stmt = nullptr;

    ~if_else() {
        delete condition;
        delete exec_stmt;
        delete else_stmt;
    }

    void accept(visitor* v) override {
        v->visit_if_else(this);
    }
}; /* class if_else */

} /* namespace cantrip::ast */

#endif /* end of include guard: IF_ELSE_H */
