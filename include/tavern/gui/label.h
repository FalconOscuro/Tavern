#ifndef LABEL_H
#define LABEL_H

#include "control.h"

#include <string>

#include "tavern/graphics/font.h"
#include "tavern/graphics/shader.h"

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

    void draw() const override;

    std::string text;

    float size = 1.f;
    graphics::font_resource font;
    graphics::shader_resource shader;

}; /* end of class label : public control */

} /* namespace tavern::gui */

#endif /* end of include guard: LABEL_H */
