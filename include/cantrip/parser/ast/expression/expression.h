#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "../node.h"

namespace cantrip::ast {
    
struct expression : public node
{
    virtual ~expression() = 0;
}; /* class expression */

} /* namespace cantrip::ast */

#endif /* end of include guard: EXPRESSION_H */
