#ifndef LABEL_H
#define LABEL_H

#include "control.h"

#include <string>

namespace tavern::gui {

class label : public control
{
public:

    label()
    {}

    label(const std::string& text):
        text(text)
    {}

    ~label() = default;

    std::string text;

}; /* end of class label : public control */

} /* namespace tavern::gui */

#endif /* end of include guard: LABEL_H */
