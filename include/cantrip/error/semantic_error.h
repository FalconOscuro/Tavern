#ifndef SEMANTIC_ERROR_H
#define SEMANTIC_ERROR_H

#include "exception.h"

#include "cantrip/ast/expression/call.h"
#include "cantrip/ast/expression/cast.h"
#include "cantrip/ast/expression/identifier.h"
#include "cantrip/ast/expression/type_check.h"

#include "cantrip/ast/statement/struct.h"
#include "cantrip/ast/statement/function.h"
#include "cantrip/ast/statement/system.h"
#include "cantrip/ast/statement/var_declare.h"
#include "cantrip/ast/statement/flow.h"

// not all strictly used in semantic analyzer,
// some ast nodes have been shifted to utilize
// hash maps, and are capable of picking up on
// redefinitions during parsing

namespace cantrip::error {

// unexpected statement?

class redefinition final : public exception {
public:

    redefinition(const ast::c_struct*    struc);
    redefinition(const ast::function*    func);
    redefinition(const ast::system*      sys);
    redefinition(const ast::var_declare* var);
    
    ~redefinition() = default;
};

class undeclared_identifier final : public exception
{
public:
    undeclared_identifier(const ast::identifier* ident);

    ~undeclared_identifier() = default;
};

class unknown_typename final : public exception
{
public:
    unknown_typename(const ast::function* func);
    unknown_typename(const ast::var_declare* var);
    unknown_typename(const ast::call* call);
    unknown_typename(const ast::cast* cast);
    unknown_typename(const ast::type_check* type_check);
    
    unknown_typename(const file_pos& pos, const ast::type& type);

    ~unknown_typename() = default;
};

class not_in_loop final : public exception
{
public:
    not_in_loop(const ast::flow* flow);

    ~not_in_loop() = default;
};

class not_structure final : public exception
{
public:

    not_structure(const file_pos& pos, const ast::type& type);
    ~not_structure() = default;
};

class no_member final : public exception
{
public:

    no_member(const ast::call* call, const ast::c_struct* struc);

    ~no_member() = default;
};

class type_not_convertible final : public exception
{
public:

    type_not_convertible(const file_pos& pos, const ast::type& from, const ast::type& to);

    ~type_not_convertible() = default;
};

class invalid_system final : public exception
{
public:

    // no args
    invalid_system(const ast::system* sys);
    // var not component type
    invalid_system(const ast::system* sys, const ast::var_declare* var);

    ~invalid_system() = default;
};

} /* namespace cantrip::error */

#endif /* end of include guard: SEMANTIC_ERROR_H */
