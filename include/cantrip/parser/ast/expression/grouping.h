#ifndef GROUPING_H
#define GROUPING_H

#include "expression.h"

#include "../visitor.h"

namespace cantrip::ast {

struct grouping : public expression
{
    expression* expr = nullptr;

    grouping(expression* expr): expr(expr)
    {}

    ~grouping() {
        delete expr;
    }

    void accept(visitor* v) override {
        v->visit_grouping(this);
    }

}; /* class grouping */

} /* namespace cantrip::ast */

#endif /* end of include guard: GROUPING_H */
