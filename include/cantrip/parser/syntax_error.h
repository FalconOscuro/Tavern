#ifndef SYNTAX_ERROR_H
#define SYNTAX_ERROR_H

#include <stdexcept>

#include "cantrip/scanner/token.h"

namespace cantrip {

class syntax_error : public std::runtime_error {
public:
    syntax_error(const token& t, const char* err_msg):
        std::runtime_error(t.to_string().append(" - ").append(err_msg))
    {}

}; /* class syntax_error */

} /* namespace cantrip */

#endif /* end of include guard: SYNTAX_ERROR_H */
