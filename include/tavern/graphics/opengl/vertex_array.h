#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H
#ifdef USE_OPENGL

#include <cstdint>

#include "../../maths/vector3.hpp"

namespace tavern::graphics::opengl {

class vertex_array
{
public:

    vertex_array(maths::vector3f vertices[], uint32_t vertex_count);

    ~vertex_array();

    void activate() const;

private:

    uint32_t m_vertex_buffer;

}; /* end of class vertex_array */

} /* end of namespace tavern::graphics::opengl */

#endif // USE_OPENGL
#endif /* end of define guard OPENGL_VERTEX_ARRAY_H */
