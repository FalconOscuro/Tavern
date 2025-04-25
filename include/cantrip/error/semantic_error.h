#ifndef SEMANTIC_ERROR_H
#define SEMANTIC_ERROR_H

#include "exception.h"

#include "cantrip/ast/expression/identifier.h"

#include "cantrip/ast/statement/struct.h"
#include "cantrip/ast/statement/function.h"
#include "cantrip/ast/statement/var_declare.h"
#include "cantrip/ast/statement/flow.h"

// not all strictly used in semantic analyzer,
// some ast nodes have been shifted to utilize
// hash maps, and are capable of picking up on
// redefinitions during parsing

namespace cantrip::error {

class redefinition : public exception {
public:

    redefinition(const ast::c_struct* struc);
    redefinition(const ast::function* func);
    redefinition(const ast::var_declare* var);
    
    ~redefinition() = default;
};

class undeclared_identifier : public exception
{
public:
    undeclared_identifier(const ast::identifier* ident);

    ~undeclared_identifier() = default;
};

class unkown_typename : public exception
{
public:
    unkown_typename(const ast::function* func);
    unkown_typename(const ast::var_declare* var);
    
    unkown_typename(const file_pos& pos, const ast::type& type);

    ~unkown_typename() = default;
};

class not_in_loop : public exception
{
public:
    not_in_loop(const ast::flow* flow);

    ~not_in_loop() = default;
};

} /* namespace cantrip::error */

#endif /* end of include guard: SEMANTIC_ERROR_H */
