#ifndef FUNCTION_H
#define FUNCTION_H

#include "statement.h"

#include <vector>
#include <cstring>

#include "var_declare.h"
#include "../visitor.h"

namespace cantrip::ast {

struct function : public statement
{
    char* return_type;
    char* name;

    // NOTE: Temporary
    // no further inheritance chain, assigment used for default param values
    std::vector<var_declare*> params;
    
    statement* body;

    function(const char* p_name, const char* p_return = nullptr)
    {
        // NOTE: Recurring code block, can be made flib function
        name = new char[std::strlen(p_name) + 1];
        std::strcpy(name, p_name);

        if (p_return != nullptr) {
            return_type = new char[std::strlen(p_return) + 1];
            std::strcpy(return_type, p_return);
        }
    }

    ~function() {
        delete[] return_type;
        delete[] name;
        delete body;

        for (auto p : params)
            delete p;
    }

    void accept(visitor* v) override {
        v->visit_function(this);
    }
}; /* class function */

} /* namespace cantrip::ast */

#endif /* end of include guard: FUNCTION_H */
