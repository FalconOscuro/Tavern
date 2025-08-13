#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

#include "expression.h"

#include "cantrip/ast/visitor.h"
#include "cantrip/ast/statement/type.h"

namespace cantrip::ast {

// just a duplicate of cast, could've done through inheritance instead?
class type_check : public expression
{
public:

    template <typename... Args>
    type_check(const file_pos& pos, Args&&... args):
        expression(pos), is_type(std::forward<Args>(args)...)
    {}

    ~type_check() = default;

    type_check(const type_check& cast) = delete;
    void operator=(const type_check& cast) = delete;

    void accept(visitor* v) override {
        v->visit_type_check(this);
    }

    u_expression_ptr expr = nullptr;

    type is_type;
};

} /* namespace cantrip::ast */



#endif /* end of define guard TYPE_CHECK_H */
