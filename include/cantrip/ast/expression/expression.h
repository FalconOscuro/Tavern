#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "../node.h"

#include <memory>

namespace cantrip::ast {
    
class expression : public node
{
protected:
    // expressions evaluated using recusive descent,
    // requiring passing token position for constructor here
    // to reduce verbosity/temporary variables
    expression(const file_pos& p_pos) {
        pos = p_pos;
    }

public:

    virtual ~expression() = default;
}; /* class expression */

typedef std::unique_ptr<expression> u_expression_ptr;

} /* namespace cantrip::ast */

#endif /* end of include guard: EXPRESSION_H */
