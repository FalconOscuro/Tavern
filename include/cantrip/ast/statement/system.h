#ifndef CANTRIP_SYSTEM_H
#define CANTRIP_SYSTEM_H

#include "statement.h"

#include <string>
#include <vector>

#include "type.h"
#include "var_declare.h"

#include "../visitor.h"

namespace cantrip::ast {

// simplified version of function ast w/out return type support
class system : public statement
{
public:
    system(const char* p_name):
        name(p_name)
    {}

    ~system() = default;

    system(const system&) = delete;
    void operator=(const system&) = delete;

    void accept(visitor* v) override {
        v->visit_system(this);
    }

    std::string name;

    // NOTE: Temporary
    // Should this make use of custom type instead of re-use of var_declare?
    // no further inheritance chain, assigment used for default param values
    // Switch to ordered_map?
    // auto add eid param?
    std::vector<std::unique_ptr<var_declare>> params;
    
    u_statement_ptr body;
}; /* class system */

} /* namespace cantrip::ast */

#endif /* end of define guard CANTRIP_SYSTEM_H */
