#ifndef AST_TYPE_H
#define AST_TYPE_H

#include "statement.h"

#include "cantrip/scanner/token.h"
#include "cantrip/parser/ast/visitor.h"

#define CORE_TYPE_DEF(t) token::t - token::CORE_TYPE_START

namespace cantrip::ast {

struct type : public statement
{
    // can be used to quickly identify core types
    enum tinfo{
        INTEGER = CORE_TYPE_DEF(TYPE_INTEGER),
        FLOAT   = CORE_TYPE_DEF(TYPE_FLOAT)  ,
        BOOLEAN = CORE_TYPE_DEF(TYPE_BOOLEAN),
        STRING  = CORE_TYPE_DEF(TYPE_STRING) ,
        UNKNOWN
    } info = UNKNOWN;

    type(token::type t);
    type(const char* s);
    ~type();

    type(const type&) = delete;
    void operator=(const type&) = delete;

    char* name = nullptr;

private:

    void set_name(const char* c);
}; /* end of struct type : public statement */

} /* end of namespace cantrip::ast */

#undef CORE_TYPE_DEF
#endif /* end of define guard AST_TYPE_H */
