#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include <limits>

namespace ecs {

typedef uint32_t entity_type;

#ifndef ENTITY_TOMBSTONE
#define ENTITY_TOMBSTONE std::numeric_limits<entity_type>::max()
#endif

#ifndef MAX_ENTITY_ID
#define MAX_ENTITY_ID (ENTITY_TOMBSTONE - 1)
#endif

static_assert(MAX_ENTITY_ID < ENTITY_TOMBSTONE, "Max ID must be less than tombstone");
static_assert(MAX_ENTITY_ID > 0, "Max ID must be greater than 0");

} /* namespace ecs */

#endif /* end of include guard: ENTITY_H */
