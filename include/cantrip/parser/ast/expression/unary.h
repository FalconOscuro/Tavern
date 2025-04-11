#ifndef UNARY_H
#define UNARY_H

#include "expression.h"

#include "../visitor.h"

namespace cantrip::ast {

struct unary : public expression
{
    enum operation_type {
        LOGICAL_NOT,
        MINUS
    } type;

    expression* expr = nullptr;

    unary(operation_type type, expression* expr):
        type(type), expr(expr)
    {}

    ~unary() {
        delete expr;
    }

    void accept(visitor* v) override {
        v->visit_unary(this);
    }
}; /* class unary */

} /* namespace cantrip::ast */

#endif /* end of include guard: UNARY_H */
