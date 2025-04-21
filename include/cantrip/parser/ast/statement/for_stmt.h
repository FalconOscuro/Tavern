#ifndef FOR_STMT_H
#define FOR_STMT_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

class for_stmt : public statement
{
public:
    for_stmt() = default;
    ~for_stmt() = default;

    for_stmt(const for_stmt&) = delete;
    void operator=(const for_stmt&) = delete;

    void accept(visitor* v) override {
        v->visit_for_stmt(this);
    }

    u_expression_ptr loop_expr = nullptr;
    u_statement_ptr body = nullptr;
}; /* class for_stmt */

} /* namespace cantrip::ast */

#endif /* end of include guard: FOR_STMT_H */
