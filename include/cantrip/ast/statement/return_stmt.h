#ifndef RETURN_STMT_H
#define RETURN_STMT_H

#include "statement.h"

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

class return_stmt : public statement
{
public:
    return_stmt() = default;
    ~return_stmt() = default;

    return_stmt(const return_stmt&) = delete;
    void operator=(const return_stmt&) = delete;

    void accept(visitor* v) override {
        v->visit_return_stmt(this);
    }

    // Expression being returned, when set to nullptr no value is returned
    u_expression_ptr returned = nullptr;
}; /* class return_stmt */

} /* namespace cantrip::ast */

#endif /* end of include guard: RETURN_STMT_H */
