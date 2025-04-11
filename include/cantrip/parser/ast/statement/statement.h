#ifndef STATEMENT_H
#define STATEMENT_H

#include "../node.h"

namespace cantrip::ast {

struct statement : public node
{
    virtual ~statement() = 0;
}; /* class statement */
    
} /* namespace cantrip::ast */

#endif /* end of include guard: STATEMENT_H */
