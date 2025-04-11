#ifndef BINARY_H
#define BINARY_H

#include "expression.h"

#include "../visitor.h"

namespace cantrip::ast {

struct binary : public expression
{
    enum operation_type {
        IS_EQUAL,
        NOT_EQUAL,

        LOGICAL_AND,
        LOGICAL_OR,

        BITWISE_AND,
        BITWISE_OR,

        LESS_THAN,
        GREATER_THAN,
        LESS_THAN_EQUAL,
        GREATER_THAN_EQUAL,

        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,

        IS,
        CAST,

        ASSIGN,
        ASSIGN_ADD,
        ASSIGN_SUBTRACT,
        ASSIGN_DIVIDE,
        ASSIGN_MULTIPLY,

        ATTRIBUTE,
        SUBSCRIPTION,
    } type;

    expression* left = nullptr;
    expression* right = nullptr;

    binary(operation_type type, expression* left, expression* right):
        type(type), left(left), right(right)
    {}
    ~binary() {
        delete left;
        delete right;
    }

    void accept(visitor* v) override {
        v->visit_binary(this);
    }
};

} /* namespace cantrip::ast */

#endif /* end of include guard: BINARY_H */
