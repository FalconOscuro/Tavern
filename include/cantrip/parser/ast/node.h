#ifndef NODE_H
#define NODE_H

namespace cantrip::ast {

class visitor;

struct node
{
    virtual ~node() = default;

    virtual void accept(visitor* v) = 0;
}; /* class node */
    
} /* namespace cantrip::ast */

#endif /* end of include guard: NODE_H */
