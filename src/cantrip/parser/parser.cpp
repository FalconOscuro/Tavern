#include "cantrip/parser/parser.h"

#include <stdexcept>

#include "cantrip/error/semantic_error.h"

namespace cantrip {

void parser::parse_module(module& module)
{
    while (!at_end())
    {
        discard_tokens();
        // for error throwing
        const token& t = peek();

        if (match(COMPONENT))
        {
            using u_component_ptr = module::u_component_ptr;
            u_component_ptr stmt = component();
            stmt->pos = t.pos;

            if (module.components.count(stmt->name))
                throw error::redefinition(stmt.get());

            module.components.emplace(std::make_pair(stmt->name, u_component_ptr(stmt.release())));
        }

        else if (match(FUNCTION))
        {
            using u_function_ptr = module::u_function_ptr;
            u_function_ptr stmt = function();
            stmt->pos = t.pos;

            if (module.functions.count(stmt->name))
                throw error::redefinition(stmt.get());

            module.functions.emplace(std::make_pair(stmt->name, u_function_ptr(stmt.release())));
        }

        else if (match(SYSTEM))
        {
            using u_system_ptr = module::u_system_ptr;
            u_system_ptr stmt = system();
            stmt->pos = t.pos;

            if (module.systems.count(stmt->name))
                throw error::redefinition(stmt.get());

            module.systems.emplace(std::make_pair(stmt->name, u_system_ptr(stmt.release())));
        }

        else if (match(CLASS))
            throw error::syntax(t, "Classes currently unsupported!");

        else if (match(STRUCT))
            throw error::syntax(t, "Structs currently unsupported!");

        // File end to maintain separation
        else if (match(FILE_END))
        {}

        else 
            throw error::syntax(t, "Invalid top-level statement!");
    }
}

// TODO:
// Need to differentiate between top-level statements.
// Top-level statements include:
// - Functions
// - Classes?? (are classes still a thing?)
// - Components
std::vector<std::unique_ptr<ast::statement>> parser::parse()
{
    std::vector<std::unique_ptr<ast::statement>> stmts;

    while (!at_end()) {
        stmts.push_back(std::unique_ptr<ast::statement>(statement()));
        discard_tokens();
    }

    // hopefully doesn't self destruct unique ptrs when returning
    return stmts;
}

} /* namespace cantrip */
