#ifndef AST_MODULE_H
#define AST_MODULE_H

#include <memory>
#include <string_view>
#include <unordered_map>

namespace cantrip::ast
{

class function;
class component;

struct module
{
    module() = default;
    ~module() = default;

    module(const module&) = delete;
    void operator=(const module&) = delete;

    using u_function_ptr  = std::unique_ptr<function>;
    using u_component_ptr = std::unique_ptr<component>;

    std::unordered_map<std::string_view, u_function_ptr> functions;

    std::unordered_map<std::string_view, u_component_ptr> components;
};

} /* namespace cantrip::ast */

#endif /* end of include guard: AST_MODULE_H */
