#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include "expression.h"

#include <cstring>

#include "../visitor.h"

namespace cantrip::ast {

struct identifier : public expression
{
    char* name;

    identifier(const char* s) {
        name = new char[std::strlen(s) + 1];
        std::strcpy(name, s);
    }

    ~identifier() {
        delete[] name;
    }

    void accept(visitor* v) override {
        v->visit_identifier(this);
    }
}; /* class identifier */

} /* namespace cantrip::ast */

#endif /* end of include guard: IDENTIFIER_H */
