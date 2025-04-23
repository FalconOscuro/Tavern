#ifndef AST_STRUCT_H
#define AST_STRUCT_H

#include "statement.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../visitor.h"
#include "function.h"
#include "var_declare.h"

namespace cantrip::ast {

class c_struct : public statement
{
public:
    c_struct(const char* p_name): name(p_name) {}
    virtual ~c_struct() = default;

    c_struct(const c_struct&) = delete;
    void operator=(const c_struct&) = delete;

    // throw exception?
    virtual void accept(visitor* v) override
    {(void)v;}

    std::string name;

    // give vars/functions an id
    // refer through id?
    // use shared ptr?

    // TODO: Switch to unordered_map
    std::vector<std::unique_ptr<var_declare>> vars;
    std::vector<std::unique_ptr<function>> funcs;

}; /* end of struct c_struct */

} /* namespace cantrip::ast */


#endif /* end of include guard: AST_STRUCT_H */
