#include "cantrip/ast/statement/struct.h"

namespace cantrip::ast {

c_struct::c_struct(const char* p_name, const struct_type s_type):
    name(p_name), s_type(s_type)
{
    // ensure first element of any component is the entity ID
    if (s_type == COMPONENT)
    {
        // NOTE: Should change to entity core type
        u_var_ptr eid_var = std::make_unique<var_declare>(CORE_INT, "eid");

        try_add_var(eid_var);
    }
}

bool c_struct::try_add_func(u_func_ptr& func)
{
    const bool success = try_add_func(func.get());

    // intentionally void here as second owning u_ptr already created
    // shouldn't lead to issues?
    if (success)
        (void)func.release();

    return success;
}

bool c_struct::try_add_var(u_var_ptr& var)
{
    const bool success = try_add_var(var.get());

    // see above try_add_func
    if (success)
        (void)var.release();

    return success;
}

bool c_struct::try_add_func(function* func)
{
    // func being null here should be impossible
    if (!func || try_get_func(func->name))
        return false;

    m_funcs.emplace_back(func);
    m_func_map.emplace(std::make_pair(func->name, func));
    return true;
}

bool c_struct::try_add_var(var_declare* var)
{
    // var being null here should be impossible
    if (!var || try_get_var(var->name))
        return false;

    m_vars.emplace_back(var);
    m_var_map.emplace(std::make_pair(var->name, var));
    return true;
}

function* c_struct::try_get_func(const std::string_view func_name)
{
    auto found = m_func_map.find(func_name);

    if (found != m_func_map.end())
        return found->second.get();

    else
        return nullptr;
}

var_declare* c_struct::try_get_var(const std::string_view var_name)
{
    auto found = m_var_map.find(var_name);

    if (found != m_var_map.end())
        return found->second.get();

    else
        return nullptr;
}

const function* c_struct::try_get_func(const std::string_view func_name) const
{
    auto found = m_func_map.find(func_name);

    if (found != m_func_map.end())
        return found->second.get();

    else
        return nullptr;
}

const var_declare* c_struct::try_get_var(const std::string_view var_name) const
{
    auto found = m_var_map.find(var_name);

    if (found != m_var_map.end())
        return found->second.get();

    else
        return nullptr;
}

} /* namespace cantrip::ast */
