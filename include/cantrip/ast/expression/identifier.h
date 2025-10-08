#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include "expression.h"

#include <string_view>

#include "../visitor.h"
#include "../statement/var_declare.h"

namespace cantrip::ast {

class identifier : public expression
{
public:
    identifier(const file_pos& pos, const char* s);
    ~identifier();

    identifier(const identifier&) = delete;
    void operator=(const identifier&) = delete;

    void accept(visitor* v) override {
        v->visit_identifier(this);
    }

    // clunky, could do via friend access and make private or make separate resolved identifier class(preffered)
    void set_var_info(const var_declare* var_info);

    const std::string_view name() const;

    bool resolved() const {
        return m_name_len == 0 && m_var_info != nullptr;
    }

private:

    // safely destroy temporary name buffer
    void destroy_name_buffer();

    union {
        char* m_name = nullptr;
        const var_declare* m_var_info;
    };

    // 0 reserved for resolved identifiers using pointer to var declare
    unsigned long m_name_len;
}; /* class identifier */

} /* namespace cantrip::ast */

#endif /* end of include guard: IDENTIFIER_H */
