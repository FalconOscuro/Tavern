#ifndef AST_TYPE_H
#define AST_TYPE_H

#include <string_view>

namespace cantrip::ast {

class c_struct;

enum type_info {
    UNRESOLVED,
    VOID, // Used for functions without return, should throw error if used elsewhere
    CORE_INT,
    CORE_FLOAT,
    CORE_STRING,
    CORE_BOOL,
    CUSTOM
};

class type
{
public:
    type();
    type(const char* type_name);
    type(type_info t_info);
    type(const type& t);

    ~type();

    type_info get_type_info() const {
        return m_type;
    }

    const std::string_view name() const;
    const c_struct* get_custom_type() const;

    void resolve(const c_struct* type);

    type& operator=(const type& t);

    bool operator==(const type& t) const;
    bool operator!=(const type& t) const;

private:
    void set_unresolved_name(const char* type_name);
    void clear_data();

    type_info m_type;

    union {
        char* unresolved_name = nullptr;
        const c_struct* custom;
    } m_data;
};

} /* namespace cantrip::ast */


#endif /* end of include guard: AST_TYPE_H */
