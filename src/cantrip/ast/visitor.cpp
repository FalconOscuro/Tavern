#include "cantrip/ast/visitor.h"

#include "cantrip/ast/node.h"

namespace cantrip::ast {

// function definition has to be done in source file to avoid circular includes in preamble
void visitor::visit(node* n) {
    n->accept(this);
}

} /* namespace cantrip::ast */
