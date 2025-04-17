#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include "expression.h"

#include <string>

#include "../visitor.h"

namespace cantrip::ast {

struct identifier : public expression
{
    identifier(const char* s): name(s) {}
    ~identifier() = default;

    void accept(visitor* v) override {
        v->visit_identifier(this);
    }

    std::string name;
}; /* class identifier */

} /* namespace cantrip::ast */

#endif /* end of include guard: IDENTIFIER_H */
