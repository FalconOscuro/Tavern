#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <vector>
#include <memory>
#include <cassert>

#include "syntax_error.h"
#include "ast/statement/statement.h"
#include "ast/expression/expression.h"

#include "ast/statement/var_declare.h"
#include "ast/statement/function.h"
#include "ast/statement/component.h"

#include "cantrip/scanner/token.h"

namespace cantrip {

struct module_info
{
    using component_ptr = std::shared_ptr<ast::component>;
    using function_ptr  = std::shared_ptr<ast::function>;

    std::vector<component_ptr> components;
    std::vector<function_ptr> functions;
}; /* end of struct module_info */

class parser
{
    using stmt_ptr = ast::u_statement_ptr;
    using expr_ptr = ast::u_expression_ptr;

    template<typename T>
    using u_ptr = std::unique_ptr<T>;

public:

    parser(std::vector<token>& tokens): m_tokens(tokens)
    {}

    module_info parse_module();

    std::vector<stmt_ptr> parse();

    expr_ptr generate_ast() {
        return expression();
    }

private:

    std::vector<token> m_tokens;
    size_t m_index = 0;

    // ==============================
    // STATEMENT PARSE FUNCTIONS
    // ==============================
    // Listed in order of priority, low to high

    stmt_ptr statement();

    stmt_ptr block_implicit();
    stmt_ptr block_explicit();

    // Does not pop tokens like standard match function does
    u_ptr<ast::var_declare> var_declare();

    stmt_ptr if_else();
    stmt_ptr while_stmt();

    stmt_ptr return_stmt();
    stmt_ptr flow();

    u_ptr<ast::function> function();
    u_ptr<ast::component> component();

    stmt_ptr expression_statement();

    // peek for type token which could either be IDENTIFER or CORE_TYPE_*
    inline bool peek_is_type() {
        return peek() == token_type::IDENTIFIER || (
                peek() >= token_type::CORE_TYPE_START 
                    && peek() <= token_type::CORE_TYPE_END
        );
    }

    // peek for var declare production which is type token followed by IDENTIFIER
    inline bool peek_is_var_declare() {
        // var declare is either coretype or user type (token::IDENTIFIER) followed by IDENTIFIER
        return (peek_is_type() && next() == token_type::IDENTIFIER);
    }

    inline void match_stmt_end() {
        if (!(match(token_type::STATEMENT_END) || match(token_type::NEW_LINE)))
            throw syntax_error(peek(), "Expected to find statement end token");
    }

    // discard newline and semicolon tokens to find next consumable token
    inline void discard_tokens() {
        while (match(token_type::STATEMENT_END) || match(token_type::NEW_LINE));
    }

    // ==============================
    // EXPRESSION PARSE FUNCTIONS
    // ==============================
    // Listed in order of priority, low to high

    expr_ptr expression();

    expr_ptr cast();

    expr_ptr logical_or();
    expr_ptr logical_and();
    expr_ptr logical_not();

    expr_ptr is_equal();
    expr_ptr not_equal();

    expr_ptr less_than();
    expr_ptr greater_than();
    expr_ptr less_than_equal();
    expr_ptr greater_than_equal();

    expr_ptr subtract();
    expr_ptr add();
    expr_ptr multiply();
    expr_ptr divide();
    expr_ptr sign();

    expr_ptr is();

    expr_ptr assign();
    expr_ptr assign_add();
    expr_ptr assign_subtract();
    expr_ptr assign_multiply();
    expr_ptr assign_divide();

    expr_ptr attribute();
    expr_ptr call();
    std::vector<expr_ptr> call_param_list();

    expr_ptr subscription();

    expr_ptr primary();

    inline expr_ptr expect_expression() {
        const token& t = peek();

        try { return expression(); }
        catch (std::exception& e) {
            throw syntax_error(t, "Expected expression");
        }
    }

    inline bool at_end() const {
        return m_index >= m_tokens.size()
            || peek() == token_type::FILE_END;
    }

    inline bool match(token_type t) {
        // Always evaluates to false if index beyond bounds of token index
        if (!at_end() && peek() == t) {
            m_index++;
            return true;
        }

        return false;
    }

    inline const token& peek() const {
        // WARNING: error if token stream empty
        assert(!m_tokens.empty() && "Tried to peek token but token stream was empty.");
        return m_tokens[m_index];
    }

    inline const token& previous() const {
        // WARNING: leads to errors if index = 0
        // rare to occur, previous used to see matched tokens
        assert(m_index > 0 && "Tried to peek previous token whilst at token stream start.");
        return m_tokens[m_index - 1];
    }

    inline const token& next() const {
        // WARNING: leads to errors if index at end
        // rare to occur, stream should always end with file end token
        assert(m_index < m_tokens.size() - 1 && "Tried to peek next token whilst at token stream end.");
        return m_tokens[m_index + 1];
    }

    inline void match_or_throw(token_type t_match, const token& t_throw, const char* err_msg) {
        if (!match(t_match))
            throw syntax_error(t_throw, err_msg);
    }

}; /* class parser */

} /* namespace cantrip */

#endif /* end of include guard: PARSER_H */
