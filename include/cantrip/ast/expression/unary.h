#ifndef UNARY_H
#define UNARY_H

#include "expression.h"
#include <memory>

#include "../visitor.h"

namespace cantrip::ast {

enum unary_operator {
    LOGICAL_NOT,
    MINUS
};

class unary : public expression
{
public:
    unary(const file_pos& pos, unary_operator type, expression* expr):
        expression(pos), type(type), expr(expr)
    {}
    ~unary() = default;

    unary(const unary&) = delete;
    void operator=(const unary&) = delete;

    void accept(visitor* v) override {
        v->visit_unary(this);
    }

    unary_operator type;
    u_expression_ptr expr = nullptr;
}; /* class unary */

} /* namespace cantrip::ast */

#endif /* end of include guard: UNARY_H */
