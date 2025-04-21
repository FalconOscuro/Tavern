#ifndef CALL_H
#define CALL_H

#include "expression.h"

#include <string>
#include <vector>

#include "../visitor.h"

namespace cantrip::ast {

class call : public expression
{
public:
    call(const file_pos& pos, const char* p_name, expression* caller = nullptr) :
        expression(pos), caller(caller), name(p_name)
    {}
    ~call() = default;

    call(const call&) = delete;
    void operator=(const call&) = delete;

    void accept(visitor* v) override {
        v->visit_call(this);
    }

    u_expression_ptr caller;
    std::string name;
    std::vector<u_expression_ptr> params;
}; /* struct call */

} /* namespace cantrip::ast */


#endif /* end of include guard: CALL_H */
