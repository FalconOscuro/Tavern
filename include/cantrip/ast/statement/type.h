#ifndef AST_TYPE_H
#define AST_TYPE_H

// need to add support for other custom data types
#include <string_view>

#include "../../scanner/token.h"

namespace cantrip::ast {

class c_struct;

enum type_classifer : unsigned short {
    UNRESOLVED,
    VOID, // Used for functions without return, should throw error if used elsewhere
    CORE_INT,
    CORE_FLOAT,
    CORE_STRING,
    CORE_BOOL,
    CORE_ENTITY,
    CUSTOM_COMPONENT,
    CUSTOM_CLASS,
    CUSTOM_STRUCT,
};

enum type_flags : unsigned short {
    NONE        = 0,
    REFERENCE   = 1 << 0,
    CONSTANT    = 1 << 1,
    ARRAY       = 1 << 2,
};

struct type_info
{
    type_info() = default;
    type_info(type_classifer classifier): classifier(classifier)
    {}

    type_info(const type_info&) = default;
    type_info& operator=(const type_info&) = default;

    type_classifer classifier = VOID;
    type_flags flags = NONE;

    bool operator==(type_classifer p_classifer) const {
        return classifier == p_classifer;
    }

    bool operator!=(type_classifer p_classifer) const {
        return classifier != p_classifer;
    }

    bool operator==(const type_info& rhs) const {
        return classifier == rhs.classifier && flags == rhs.flags;
    }

    bool operator!=(const type_info& rhs) const {
        return classifier != rhs.classifier || flags != rhs.flags;
    }
};

class type
{
public:
    type();
    type(const char* type_name);
    type(type_classifer t_info);
    type(token_type t_type);
    type(const type& t);

    ~type();

    type_info get_type_info() const {
        return m_type;
    }

    const std::string_view name() const;
    c_struct* get_custom_type() const;

    void resolve(c_struct* type);

    type& operator=(const type& t);

    bool operator==(const type& t) const;
    bool operator!=(const type& t) const;

    bool operator==(type_classifer t) const;
    bool operator!=(type_classifer t) const;

    bool is_resolved_custom_type() const;
    bool is_custom_type_or_unresolved() const;

    unsigned long int get_size() const;

    // if array size is 0, then this is not an array
    // a bit hacky but works
    // NOTE: UNREPRESENTED W/ STRING NAME!!
    uint32_t array_size = 0;

private:
    void set_unresolved_name(const char* type_name);
    void clear_data();

    type_info m_type;

    union {
        char* unresolved_name = nullptr;
        c_struct* custom;
    } m_data;
};

} /* namespace cantrip::ast */


#endif /* end of include guard: AST_TYPE_H */
