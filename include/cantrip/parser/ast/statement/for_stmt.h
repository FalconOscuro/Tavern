#ifndef FOR_STMT_H
#define FOR_STMT_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

struct for_stmt : public statement
{
    expression* loop_expr = nullptr;
    statement* body = nullptr;

    ~for_stmt() {
        delete loop_expr;
        delete body;
    }

    void accept(visitor* v) override {
        v->visit_for_stmt(this);
    }
}; /* class for_stmt */

} /* namespace cantrip::ast */

#endif /* end of include guard: FOR_STMT_H */
