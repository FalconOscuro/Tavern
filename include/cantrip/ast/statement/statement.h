#ifndef STATEMENT_H
#define STATEMENT_H

#include "../node.h"

#include <memory>

namespace cantrip::ast {

class statement : public node
{
public:
    virtual ~statement() = default;
}; /* class statement */

typedef std::unique_ptr<statement> u_statement_ptr;
    
} /* namespace cantrip::ast */

#endif /* end of include guard: STATEMENT_H */
