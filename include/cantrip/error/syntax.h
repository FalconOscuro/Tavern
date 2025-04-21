#ifndef SYNTAX_ERROR_H
#define SYNTAX_ERROR_H

#include "exception.h"
#include "cantrip/scanner/token.h"

namespace cantrip::error {

class syntax : public exception {
public:
    syntax(const token& t, const char* err_msg):
        exception(t.pos, t.to_string().append(" - ").append(err_msg).c_str())
    {}

    ~syntax() = default;

}; /* class syntax_error */

} /* namespace cantrip */

#endif /* end of include guard: SYNTAX_ERROR_H */
