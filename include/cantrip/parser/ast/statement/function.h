#ifndef FUNCTION_H
#define FUNCTION_H

#include "statement.h"

#include <string>
#include <vector>

#include "var_declare.h"
#include "../visitor.h"

namespace cantrip::ast {

class function : public statement
{
public:
    function(const char* p_name, const char* p_return = nullptr) {
        // NOTE: Recurring code block, can be made flib function
        name = std::string(p_name);

        if (p_return != nullptr)
            return_type = std::string(p_return);
    }

    ~function() = default;

    function(const function&) = delete;
    void operator=(const function&) = delete;

    void accept(visitor* v) override {
        v->visit_function(this);
    }

    std::string return_type;
    std::string name;

    // NOTE: Temporary
    // Should this make use of custom type instead of re-use of var_declare?
    // no further inheritance chain, assigment used for default param values
    // TODO: Switch to unordered_map
    std::vector<std::unique_ptr<var_declare>> params;
    
    u_statement_ptr body;
}; /* class function */

} /* namespace cantrip::ast */

#endif /* end of include guard: FUNCTION_H */
