#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "../node.h"

#include <memory>

namespace cantrip::ast {
    
struct expression : public node
{
    virtual ~expression() = default;
}; /* class expression */

typedef std::unique_ptr<expression> u_expression_ptr;

} /* namespace cantrip::ast */

#endif /* end of include guard: EXPRESSION_H */
