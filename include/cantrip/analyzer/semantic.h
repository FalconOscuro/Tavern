#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "cantrip/ast/visitor.h"

#include "cantrip/module/module.h"

#include <string_view>

#include "cantrip/scanner/file_pos.h"
#include "environment.h"

namespace cantrip::analyzer {

class semantic final : public ast::visitor
{
public:
    semantic() = default;
    virtual ~semantic() = default;

    void analyze_module(module* module);

protected:

    // expressions
    void visit_binary(ast::binary*) override;
    void visit_call(ast::call*) override;
    void visit_cast(ast::cast*) override;
    void visit_core_type(ast::core_type*) override;
    void visit_grouping(ast::grouping*) override;
    void visit_identifier(ast::identifier*) override;
    void visit_literal(ast::literal*) override;
    void visit_type_check(ast::type_check*) override;
    void visit_unary(ast::unary*) override;

    // statements
    void visit_block(ast::block*) override;
    void visit_component(ast::component*) override;
    void visit_expr_stmt(ast::expr_stmt*) override;
    void visit_flow(ast::flow*) override;
    void visit_for_stmt(ast::for_stmt*) override;
    void visit_function(ast::function*) override;
    void visit_if_else(ast::if_else*) override;
    void visit_return_stmt(ast::return_stmt*) override;
    void visit_system(ast::system*) override;
    void visit_var_declare(ast::var_declare*) override;
    void visit_while_stmt(ast::while_stmt*) override;

private:

    void resolve_struct_vars(ast::c_struct* c_struct);

    void resolve_func_return_type(ast::function* func);
    bool resolve_type(ast::type& type);
    void resolve_non_void_type_or_throw(ast::type& type, const file_pos& pos);

    // todo: proper type conversion checking
    bool is_type_convertible(const ast::type& from, const ast::type& to);

    environment_stack m_env_stack;
    ast::type m_type;

    // currently only used for member function lookup within struct
    // could be useful later on if self/this keyword added
    ast::component* m_self_env;

    // scope for current module being explored, for member function resolution
    module* m_module = nullptr;

    // scope for current function being explored
    // need to change to support systems?
    ast::function* m_function = nullptr;
}; /* end of class semantic_analyzer */

} /* namespace cantrip::analyzer */


#endif /* end of include guard: SEMANTIC_ANALYZER_H */
