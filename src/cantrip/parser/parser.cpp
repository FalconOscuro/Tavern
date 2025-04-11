#include "cantrip/parser/parser.h"
#include <stdexcept>

namespace cantrip {

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
