#ifndef GENERIC_TEXTURE_H
#define GENERIC_TEXTURE_H

#include <cstdlib>
#include <memory>

#include <glm/vec3.hpp>

namespace tavern::graphics {

// unique ptr calls free
//std::unique_ptr<unsigned char, decltype(std::free)*> load_image_data(const char* path, glm::ivec3& dimensions);

} /* end of namespace tavern::graphics */

#endif /* end of define guard GENERIC_TEXTURE_H */
