#ifndef BINARY_H
#define BINARY_H

#include "expression.h"

#include "../visitor.h"

namespace cantrip::ast {

enum binary_operator {
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

    ASSIGN,
    ASSIGN_ADD,
    ASSIGN_SUBTRACT,
    ASSIGN_DIVIDE,
    ASSIGN_MULTIPLY,

    ATTRIBUTE,
    SUBSCRIPTION,
};

class binary : public expression
{
public:
    binary(const file_pos& pos, binary_operator type, expression* left, expression* right):
        expression(pos), type(type), left(left), right(right)
    {}
    ~binary() = default;

    binary(const binary&) = delete;
    void operator=(const binary&) = delete;

    void accept(visitor* v) override {
        v->visit_binary(this);
    }

    binary_operator type;

    u_expression_ptr left = nullptr;
    u_expression_ptr right = nullptr;
};

} /* namespace cantrip::ast */

#endif /* end of include guard: BINARY_H */
