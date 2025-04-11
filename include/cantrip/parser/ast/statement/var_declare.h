#ifndef VAR_DECLARE_H
#define VAR_DECLARE_H

#include "statement.h"

#include <cstring>

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

struct var_declare : public statement
{
    // TODO: Could be replaced by ID system, would require scope tracking through parsing to maintain unmagled name for runtime referencing
    char* type;
    char* name;

    expression* expr = nullptr;

    var_declare(const char* p_type, const char* p_name) {
        type = new char[std::strlen(p_type) + 1];
        name = new char[std::strlen(p_name) + 1];

        std::strcpy(type, p_type);
        std::strcpy(name, p_name);
    }

    ~var_declare() {
        delete expr;
        delete type;
        delete name;
    }

    void accept(visitor* v) override {
        v->visit_var_declare(this);
    }
}; /* class var_declare */

} /* namespace cantrip::ast */

#endif /* end of include guard: VAR_DECLARE_H */
