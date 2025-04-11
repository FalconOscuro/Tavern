#ifndef CORE_TYPE_H
#define CORE_TYPE_H

#include "expression.h"

#include "cantrip/scanner/token.h"
#include "../visitor.h"

namespace cantrip::ast {

struct core_type : public expression
{
    enum c_type {
        INTEGER = token::TYPE_INTEGER,
        BOOLEAN = token::TYPE_BOOLEAN,
        FLOAT   = token::TYPE_FLOAT  ,
        STRING  = token::TYPE_STRING
    } type;

    core_type(token::type type) : type((c_type)type)
    {
        // assert?
    }

    void accept(visitor* v) override {
        v->visit_core_type(this);
    }
}; /* class core_type */

} /* namespace cantrip::ast */

#endif /* end of include guard: CORE_TYPE_H */
