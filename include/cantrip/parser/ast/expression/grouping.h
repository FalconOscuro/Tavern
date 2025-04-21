#ifndef GROUPING_H
#define GROUPING_H

#include "expression.h"

#include "../visitor.h"

namespace cantrip::ast {

class grouping : public expression
{
public:
    grouping(const file_pos& pos, expression* expr):
       expression(pos), expr(expr)
    {}
    ~grouping() = default;

    grouping(const grouping&) = delete;
    void operator=(const grouping&) = delete;

    void accept(visitor* v) override {
        v->visit_grouping(this);
    }

    u_expression_ptr expr = nullptr;
}; /* class grouping */

} /* namespace cantrip::ast */

#endif /* end of include guard: GROUPING_H */
