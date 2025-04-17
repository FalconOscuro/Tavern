#ifndef LITERAL_H
#define LITERAL_H

#include "expression.h"

#include "../visitor.h"

namespace cantrip::ast {

enum literal_type {
    TRUE,
    FALSE,
    LIT_NULL,
    STRING,
    INTEGER,
    FLOAT
};

struct literal : public expression
{
private:
    literal_type m_type = LIT_NULL;

    void clear_data() {
        if (m_type == STRING)
            delete[] m_data.string;
    }

    union {
        char* string = nullptr;
        int integer;
        float floating;
    } m_data;

public:

    literal(): m_type(LIT_NULL) {}

    literal(bool b) {
        m_type = (b ? TRUE : FALSE);
    }

    literal(int i): m_type(INTEGER) {
        m_data.integer = i;
    }

    literal(float f): m_type(FLOAT) {
        m_data.floating = f;
    }

    literal(const char* c);

    ~literal() {
        clear_data();
    }

    void accept(visitor* v) override {
        v->visit_literal(this);
    }

    inline literal_type get_type() const {
        return m_type;
    }

    int get_integer() const;
    float get_float() const;
    const char* get_string() const;
}; /* class literal */

} /* namespace cantrip::ast */

#endif /* end of include guard: LITERAL_H */
