#include "cantrip/parser/ast/node.h"
#include "cantrip/parser/ast/expression/expression.h"
#include "cantrip/parser/ast/statement/statement.h"

namespace cantrip::ast {

// required for semantics to allow base node types to be pure virtual
node::~node() {}
expression::~expression() {}
statement::~statement() {}

} /* namespace cantrip::ast */
