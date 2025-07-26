#ifndef AST_MODULE_H
#define AST_MODULE_H

#include <memory>
#include <string_view>
#include <unordered_map>

#include "info.h"

// move this
namespace cantrip
{

namespace ast {

class function;
class component;


} /* namespace ast */

struct module
{
    module() = default;
    ~module() = default;

    module(module&&) = default;

    module(const module&) = delete;
    void operator=(const module&) = delete;

    using u_function_ptr  = std::unique_ptr<ast::function>;
    using u_component_ptr = std::unique_ptr<ast::component>;
    // struct and class

    std::unordered_map<std::string_view, u_function_ptr> functions;

    std::unordered_map<std::string_view, u_component_ptr> components;

    module_info info;
};

} /* namespace cantrip */

#endif /* end of include guard: AST_MODULE_H */
