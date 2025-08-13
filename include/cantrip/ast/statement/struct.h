#ifndef AST_STRUCT_H
#define AST_STRUCT_H

#include "statement.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../visitor.h"
#include "function.h"
#include "var_declare.h"

namespace cantrip::ast {

enum struct_type {
    CLASS,
    STRUCT,
    COMPONENT
};

class c_struct : public statement
{
public:
    using u_var_ptr = std::unique_ptr<var_declare>;
    using u_func_ptr = std::unique_ptr<function>;

    using var_declare_container = std::vector<u_var_ptr>;
    using function_containter = std::vector<u_func_ptr>;

    using var_iterator = var_declare_container::iterator;
    using const_var_iterator = var_declare_container::const_iterator;

    using func_iterator = function_containter::iterator;
    using const_func_iterator = function_containter::const_iterator;

    using var_declare_map = std::unordered_map<std::string, var_declare*>;
    using function_map = std::unordered_map<std::string, function*>;

    c_struct(const char* p_name, const struct_type s_type);

    virtual ~c_struct() = default;

    c_struct(const c_struct&) = delete;
    void operator=(const c_struct&) = delete;

    // throw exception?
    virtual void accept(visitor* v) override
    {(void)v;}

    // private?
    std::string name;
    struct_type s_type;

    // overloads for unique_ptrs
    // will take ownership if add is successful
    bool try_add_func(u_func_ptr& func);
    bool try_add_var(u_var_ptr& var);

    function* try_get_func(const std::string_view func_name);
    var_declare* try_get_var(const std::string_view var_name);

    const function* try_get_func(const std::string_view func_name) const;
    const var_declare* try_get_var(const std::string_view var_name) const;

    /// iterators
    // var
    inline var_iterator vars_begin() noexcept {
        return m_vars.begin();
    }

    inline const_var_iterator vars_begin() const noexcept {
        return m_vars.cbegin();
    }

    inline const_var_iterator vars_cbegin() const noexcept {
        return m_vars.cbegin();
    }

    inline var_iterator vars_end() noexcept {
        return m_vars.end();
    }

    inline const_var_iterator vars_end() const noexcept {
        return m_vars.cend();
    }

    inline const_var_iterator vars_cend() const noexcept {
        return m_vars.cend();
    }

    // function
    inline func_iterator funcs_begin() noexcept {
        return m_funcs.begin();
    }

    inline const_func_iterator funcs_begin() const noexcept {
        return m_funcs.cbegin();
    }

    inline const_func_iterator funcs_cbegin() const noexcept {
        return m_funcs.cbegin();
    }

    inline func_iterator funcs_end() noexcept {
        return m_funcs.end();
    }

    inline const_func_iterator funcs_end() const noexcept {
        return m_funcs.cend();
    }

    inline const_func_iterator funcs_cend() const noexcept {
        return m_funcs.cend();
    }

private:

    // give vars/functions an id
    // refer through id?
    var_declare_container m_vars = var_declare_container();
    function_containter m_funcs = function_containter();

    var_declare_map m_var_map = var_declare_map();
    function_map m_func_map = function_map();

}; /* end of struct c_struct */

} /* namespace cantrip::ast */


#endif /* end of include guard: AST_STRUCT_H */
