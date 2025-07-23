#ifndef AST_COMPONENT_H
#define AST_COMPONENT_H

#include "struct.h"

#include "../visitor.h"

namespace cantrip::ast {

class component : public c_struct
{
public:
    component(const char* p_name): c_struct(p_name, COMPONENT)
    {}
    ~component() = default;

    component(const component&) = delete;
    void operator=(const component&) = delete;

    void accept(visitor* v) override {
        v->visit_component(this);
    }
}; /* class component */

} /* namespace cantrip::ast */

#endif /* end of include guard: AST_COMPONENT_H */
