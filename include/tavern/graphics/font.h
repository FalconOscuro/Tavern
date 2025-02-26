#ifndef FONT_H
#define FONT_H

#include "opengl/font.h"

#include "tavern/resource/resource_ptr.hpp"

namespace tavern::graphics {

typedef RENDERER_NS::font font;
typedef resource::resource_ptr<font> font_resource;

} /* end of namespace tavern::graphics */

#endif /* end of define guard FONT_H */
