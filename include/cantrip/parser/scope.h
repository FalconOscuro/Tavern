#ifndef SCOPE_H
#define SCOPE_H

#include <memory>
#include <stack>
#include <string_view>
#include <unordered_map>

#include "ast/statement/type.h"
#include "ast/statement/function.h"
#include "ast/statement/var_declare.h"

namespace cantrip {

struct scope
{
    std::unordered_map<std::string_view, std::shared_ptr<ast::function>> functions;

    std::unordered_map<std::string_view, std::shared_ptr<ast::var_declare>> variables;
}; /* end of struct scope */

class scope_stack
{
    // type declarations always exist at top of stack?
    // need to account for later inclusing of potential using directives
    std::unordered_map<std::string_view, std::shared_ptr<ast::type>> types;

    // scope is kept as a stack, when searching if a function/variable is in the current scope, will traverse down the stack
    // what about member functions?
    // - custom types can have function definitions within which functions should in local scope
    // - global functions exist at top of stack
    std::stack<scope> stack;
}; /* end of class scope_stack */

} /* end of namespace cantrip */

#endif /* end of define guard SCOPE_H */
