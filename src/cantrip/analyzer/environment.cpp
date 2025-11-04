#include "cantrip/analyzer/environment.h"

#include "cantrip/ast/expression/identifier.h"

namespace cantrip::analyzer {

bool environment_stack::push_var(const ast::var_declare* var)
{
    // always ensure stack is populated
    if (env_stack.empty())
        push();

    environment& env = top();

    const std::string_view name = var->name;

    // redefinition
    if (env.variables.find(name.data()) != env.variables.end())
        return false;

    else {
        env.variables.emplace(std::make_pair(name, var));
        return true;
    }
}

const ast::var_declare* environment_stack::check_identifier(const ast::identifier* identifier) const
{
    for (size_t i = 0; i < env_stack.size(); ++i) {
        const environment& env = env_stack[env_stack.size() - (i + 1)];
        auto found = env.variables.find(identifier->name().data());

        if (found != env.variables.end())
            return found->second;
    }

    return nullptr;
}

bool environment_stack::in_loop() const
{
    for (size_t i = 0; i < env_stack.size(); ++i)
        if (env_stack[i].is_loop)
            return true;

    return false;
}

} /* namespace cantrip::analyzer */
