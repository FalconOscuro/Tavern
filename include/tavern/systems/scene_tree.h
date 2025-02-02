#ifndef SCENE_TREE_H
#define SCENE_TREE_H

#include <string>

#include <ecs/ecs.h>
#include <ecs/containers/sparse_set.hpp>

namespace tavern::system {

class scene_tree
{
public:

    void update(ecs::registry& reg);

    void load_scene(const std::string& file, ecs::registry& reg);

private:

    // NOTE: Could do faster implementation by sorting sparse map itself
    // would require rework of sparse map adding indirection and pooling
    // to prevent entire structure being copied during swap operations
    ecs::container::sparse_set<> m_entities;
}; /* end of class scene_tree */

} /* end of namespace tavern::system */

#endif /* end of define guard SCENE_TREE_H */
