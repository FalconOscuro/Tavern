#ifndef FLOW_H
#define FLOW_H

#include "statement.h"

#include "../visitor.h"

namespace cantrip::ast {

class flow : public statement
{
public:
    bool is_continue;

    flow(bool is_continue) : is_continue(is_continue)
    {}
    ~flow() = default;

    void accept(visitor* v) override {
        v->visit_flow(this);
    }
}; /* class flow */

} /* namespace cantrip::ast */

#endif /* end of include guard: FLOW_H */
