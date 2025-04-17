#ifndef CORE_TYPE_H
#define CORE_TYPE_H

#include "expression.h"

#include "cantrip/scanner/token.h"
#include "../visitor.h"

namespace cantrip::ast {

struct core_type : public expression
{
    enum c_type {
        INTEGER = token_type::TYPE_INTEGER,
        BOOLEAN = token_type::TYPE_BOOLEAN,
        FLOAT   = token_type::TYPE_FLOAT  ,
        STRING  = token_type::TYPE_STRING
    } type;

    core_type(token_type type) : type((c_type)type)
    {}
    ~core_type() = default;

    void accept(visitor* v) override {
        v->visit_core_type(this);
    }
}; /* class core_type */

} /* namespace cantrip::ast */

#endif /* end of include guard: CORE_TYPE_H */
