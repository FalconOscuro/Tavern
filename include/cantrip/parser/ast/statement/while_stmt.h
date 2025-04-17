#ifndef WHILE_STMT_H
#define WHILE_STMT_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

struct while_stmt : public statement
{
    while_stmt() = default;
    ~while_stmt() = default;

    while_stmt(const while_stmt&) = delete;
    void operator=(const while_stmt&) = delete;

    void accept(visitor* v) override {
        v->visit_while_stmt(this);
    }

    u_expression_ptr condition = nullptr;
    u_statement_ptr exec_stmt = nullptr;
}; /* class while_stmt */

} /* namespace cantrip::ast */

#endif /* end of include guard: WHILE_STMT_H */
