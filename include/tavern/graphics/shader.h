#ifndef SHADER_H
#define SHADER_H

#include "opengl/shader.h"
#include "tavern/resource/resource_ptr.hpp"

namespace tavern::graphics {

typedef RENDERER_NS::shader shader;
typedef resource::resource_ptr<shader> shader_resource;

} /* end of namespace tavern::graphics */

#endif /* end of define guard SHADER_H */
