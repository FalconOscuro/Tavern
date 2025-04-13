#ifndef CALL_H
#define CALL_H

#include "expression.h"

#include <vector>
#include <cstring>

#include "../visitor.h"

namespace cantrip::ast {

struct call : public expression
{
    expression* caller;
    char* name;
    std::vector<expression*> params;

    call(const char* p_name, expression* caller = nullptr) :
        caller(caller)
    {
        name = new char[std::strlen(p_name) + 1];
        std::strcpy(name, p_name);
    }

    ~call() {
        delete caller;
        delete[] name;
        for (auto p : params)
            delete p;
    }

    void accept(visitor* v) override {
        v->visit_call(this);
    }
}; /* struct call */

} /* namespace cantrip::ast */


#endif /* end of include guard: CALL_H */
