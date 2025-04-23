#ifndef VAR_DECLARE_H
#define VAR_DECLARE_H

#include "statement.h"

#include <string>

#include "../expression/expression.h"
#include "../visitor.h"

namespace cantrip::ast {

class c_struct;

enum var_type_info {
    UNRESOLVED,
    CORE_INT,
    CORE_FLOAT,
    CORE_STRING,
    CORE_BOOL,
    CUSTOM
};

class var_type
{
public:
    var_type();
    var_type(const char* type_name);
    var_type(var_type_info type_info);
    var_type(const var_type& t);

    ~var_type();

    var_type_info type_info() const {
        return m_type;
    }

    const std::string_view name() const;
    const c_struct* get_custom_type() const;

    void resolve(const c_struct* type);

    var_type& operator=(const var_type& t);

    bool operator==(const var_type& t) const;
    bool operator!=(const var_type& t) const;

private:
    void set_unresolved_name(const char* type_name);
    void clear_data();

    var_type_info m_type;

    union {
        char* unresolved_name = nullptr;
        const c_struct* custom;
    } m_data;
};

class var_declare : public statement
{
public:
    var_declare(const char* p_type, const char* p_name);
    ~var_declare() = default;

    var_declare(const var_declare&) = delete;
    void operator=(const var_declare&) = delete;

    void accept(visitor* v) override {
        v->visit_var_declare(this);
    }


    var_type type;
    std::string name;

    u_expression_ptr expr = nullptr;
}; /* class var_declare */

} /* namespace cantrip::ast */

#endif /* end of include guard: VAR_DECLARE_H */
