#ifndef CANTRIP_EXCEPTION_H
#define CANTRIP_EXCEPTION_H

#include "cantrip/scanner/file_pos.h"

namespace cantrip::error {

class exception
{
public:
    exception(const file_pos& pos, const char* err_msg);
    virtual ~exception() = default;

    const char* what() const noexcept;

    const file_pos& get_pos() const {
        return m_pos;
    }

private:

    const file_pos m_pos;
    std::string m_err_str;
};

} /* namespace cantrip::error */

#endif /* end of include guard: CANTRIP_EXCEPTION_H */
