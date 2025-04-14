#ifndef AST_COMPONENT_H
#define AST_COMPONENT_H

#include "statement.h"

#include <cstring>
#include <unordered_map>
#include <vector>

#include "../visitor.h"
#include "function.h"
#include "var_declare.h"

namespace cantrip::ast {

struct component : public statement
{
    char* name;

    // give vars/functions an id
    // refer through id?
    // use shared ptr?

    std::vector<var_declare*> vars;
    std::vector<function*> funcs;

    component(const char* p_name) {
        name = new char[std::strlen(p_name) + 1];
        std::strcpy(name, p_name);
    }

    ~component() {
        delete[] name;

        for (auto v : vars)
            delete v;

        for (auto f : funcs)
            delete f;
    }

    void accept(visitor* v) override {
        v->visit_component(this);
    }
}; /* class component */

} /* namespace cantrip::ast */

#endif /* end of include guard: AST_COMPONENT_H */
