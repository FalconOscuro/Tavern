#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "cantrip/ast/statement/var_declare.h"

#include <deque>
#include <string>
#include <unordered_map>

namespace cantrip::analyzer {

// defines the scope for a set of variables/functions
struct environment
{
    // need better way for storing variable types?
    std::unordered_map<std::string, const ast::var_declare*> variables;

    bool is_loop = false;
};

struct environment_stack
{
    // check for matching var declares at this level
    bool push_var(const ast::var_declare* var);
    const ast::var_declare* check_identifier(const ast::identifier* identifier) const;

    bool in_loop() const;

    void push(bool is_loop = false) {
        env_stack.emplace_back().is_loop = is_loop;
    }

    void pop() {
        env_stack.pop_back();
    }

    environment& top() {
        return env_stack.back();
    };

    std::deque<environment> env_stack;

    // Store current type on top of stack for expression resolution
};

} /* namespace cantrip::analyzer */

#endif /* end of include guard: ENVIRONMENT_H */
