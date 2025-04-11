#ifndef PARSER_H
#define PARSER_H

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

class parser {
public:

    parser(std::vector<token>& tokens): m_tokens(tokens)
    {}

    std::vector<std::unique_ptr<ast::statement>> parse();

    ast::expression* generate_ast() {
        return expression();
    }

private:

    std::vector<token> m_tokens;
    size_t m_index = 0;

    // ==============================
    // STATEMENT PARSE FUNCTIONS
    // ==============================
    // Listed in order of priority, low to high

    using stmt_ptr = ast::statement*;

    stmt_ptr statement();

    stmt_ptr block_implicit();
    stmt_ptr block_explicit();

    // Does not pop tokens like standard match function does
    ast::var_declare* var_declare();

    stmt_ptr if_else();
    stmt_ptr while_stmt();

    stmt_ptr return_stmt();
    stmt_ptr flow();

    ast::function* function();
    ast::component* component();

    stmt_ptr expression_statement();

    // peek for type token which could either be IDENTIFER or CORE_TYPE_*
    bool peek_is_type() {
        return peek() == token::IDENTIFIER || (
                peek() >= token::CORE_TYPE_START 
                    && peek() <= token::CORE_TYPE_END
        );
    }

    // peek for var declare production which is type token followed by IDENTIFIER
    bool peek_is_var_declare() {
        // var declare is either coretype or user type (token::IDENTIFIER) followed by IDENTIFIER
        return (peek_is_type() && next() == token::IDENTIFIER);
    }

    void match_stmt_end() {
        if (!(match(token::STATEMENT_END) || match(token::NEW_LINE)))
            throw syntax_error(peek(), "Expected to find statement end token");
    }

    // discard newline and semicolon tokens to find next consumable token
    void discard_tokens() {
        while (match(token::STATEMENT_END) || match(token::NEW_LINE));
    }

    // ==============================
    // EXPRESSION PARSE FUNCTIONS
    // ==============================
    // Listed in order of priority, low to high

    using expr_ptr = ast::expression*;

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

    expr_ptr expect_expression() {
        const token& t = peek();

        try { return expression(); }
        catch (std::exception& e) {
            throw syntax_error(t, "Expected expression");
        }
    }

    bool at_end() const {
        return m_index >= m_tokens.size()
            || peek() == token::FILE_END;
    }

    bool match(token::type t) {
        // Always evaluates to false if index beyond bounds of token index
        if (!at_end() && peek() == t) {
            m_index++;
            return true;
        }

        return false;
    }

    const token& peek() const {
        // WARNING: error if token stream empty
        assert(!m_tokens.empty() && "Tried to peek token but token stream was empty.");
        return m_tokens[m_index];
    }

    const token& previous() const {
        // WARNING: leads to errors if index = 0
        // rare to occur, previous used to see matched tokens
        assert(m_index > 0 && "Tried to peek previous token whilst at token stream start.");
        return m_tokens[m_index - 1];
    }

    const token& next() const {
        // WARNING: leads to errors if index at end
        // rare to occur, stream should always end with file end token
        assert(m_index < m_tokens.size() - 1 && "Tried to peek next token whilst at token stream end.");
        return m_tokens[m_index + 1];
    }

    void match_or_throw(token::type t_match, const token& t_throw, const char* err_msg) {
        if (!match(t_match))
            throw syntax_error(t_throw, err_msg);
    }

}; /* class parser */

} /* namespace cantrip */

#endif /* end of include guard: PARSER_H */
