#ifndef SEMANTIC_ERROR_H
#define SEMANTIC_ERROR_H

#include "exception.h"

#include "cantrip/parser/ast/expression/identifier.h"
#include "cantrip/parser/ast/statement/var_declare.h"

namespace cantrip::error {

class redefinition : public exception {
public:

    redefinition(const ast::var_declare* var);
    ~redefinition() = default;
};

class undeclared_identifier : public exception
{
public:

    undeclared_identifier(const ast::identifier* ident);
    ~undeclared_identifier() = default;
};

} /* namespace cantrip::error */

#endif /* end of include guard: SEMANTIC_ERROR_H */
