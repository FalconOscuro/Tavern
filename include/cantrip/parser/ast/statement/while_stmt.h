#ifndef WHILE_STMT_H
#define WHILE_STMT_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

struct while_stmt : public statement
{
    expression* condition = nullptr;
    statement* exec_stmt = nullptr;

    ~while_stmt() {
        delete condition;
        delete exec_stmt;
    }

    void accept(visitor* v) override {
        v->visit_while_stmt(this);
    }
}; /* class while_stmt */

} /* namespace cantrip::ast */

#endif /* end of include guard: WHILE_STMT_H */
