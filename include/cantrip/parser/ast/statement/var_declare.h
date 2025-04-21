#ifndef VAR_DECLARE_H
#define VAR_DECLARE_H

#include "statement.h"

#include <string>

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

class var_declare : public statement
{
public:
    var_declare(const char* p_type, const char* p_name):
        type(p_type), name(p_name)
    {}
    ~var_declare() = default;

    var_declare(const var_declare&) = delete;
    void operator=(const var_declare&) = delete;

    void accept(visitor* v) override {
        v->visit_var_declare(this);
    }

    // TODO: Extend in future for implementing proper type resolution through ast nodes
    const std::string_view get_type_name() const {
        return type;
    }

    // TODO: Could be replaced by ID system, would require scope tracking through parsing to maintain unmagled name for runtime referencing
    std::string type;
    std::string name;

    u_expression_ptr expr = nullptr;
}; /* class var_declare */

} /* namespace cantrip::ast */

#endif /* end of include guard: VAR_DECLARE_H */
