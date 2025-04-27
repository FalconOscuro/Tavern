#ifndef CAST_H
#define CAST_H

#include "expression.h"

#include "cantrip/ast/visitor.h"
#include "cantrip/ast/statement/type.h"

namespace cantrip::ast {

class cast : public expression
{
public:

    cast(const file_pos& pos):
        expression(pos)
    {}

    ~cast() = default;

    cast(const cast& cast) = delete;
    void operator=(const cast& cast) = delete;

    void accept(visitor* v) override {
        v->visit_cast(this);
    }

    u_expression_ptr expr = nullptr;

    type as_type;
};


} /* namespace cantrip::ast */


#endif /* end of include guard: CAST_H */
