#ifndef SCENE_TREE_H
#define SCENE_TREE_H

#include "ecs/containers/sparse_set.hpp"
#include <memory>
#include <vector>

#include <ecs/ecs.h>

namespace tavern::system {

class scene_tree
{
public:

    void update(ecs::registry& reg);

private:

    // NOTE: Could do faster implementation by sorting sparse map itself
    // would require rework of sparse map adding indirection and pooling
    // to prevent entire structure being copied during swap operations
    ecs::container::sparse_set<> m_entities;
}; /* end of class scene_tree */

} /* end of namespace tavern::system */

#endif /* end of define guard SCENE_TREE_H */
