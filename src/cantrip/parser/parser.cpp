#include "cantrip/parser/parser.h"
#include <stdexcept>

namespace cantrip {

module_info parser::parse_module()
{
    module_info info;

    while (!at_end())
    {
        if (match(token::COMPONENT))
            info.components.emplace_back(component());

        else if (match(token::FUNCTION))
            info.functions.emplace_back(function());

        else 
            throw syntax_error(peek(), "Invalid top-level statement!");
    }

    return info;
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
