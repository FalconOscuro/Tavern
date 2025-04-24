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

class literal : public expression
{
public:

    literal(const file_pos& pos): 
        expression(pos), m_type(LIT_NULL)
    {}

    literal(const file_pos& pos, bool b): expression(pos) {
        m_type = (b ? TRUE : FALSE);
    }

    literal(const file_pos& pos, int i):
        expression(pos), m_type(INTEGER)
    {
        m_data.integer = i;
    }

    literal(const file_pos& pos, float f):
        expression(pos), m_type(FLOAT)
    {
        m_data.floating = f;
    }

    literal(const file_pos& pos, const char* c);

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
}; /* class literal */

} /* namespace cantrip::ast */

#endif /* end of include guard: LITERAL_H */
