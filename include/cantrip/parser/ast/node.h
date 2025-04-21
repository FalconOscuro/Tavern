#ifndef NODE_H
#define NODE_H

#include "cantrip/scanner/file_pos.h"

namespace cantrip::ast {

class visitor;

class node
{
public:

    virtual ~node() = default;

    virtual void accept(visitor* v) = 0;

    file_pos pos;
}; /* class node */
    
} /* namespace cantrip::ast */

#endif /* end of include guard: NODE_H */
