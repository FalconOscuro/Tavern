#ifndef SCENE_H
#define SCENE_H

#include <string>

#include <ecs/ecs.h>

namespace tavern::system {

class scene
{
public:

    scene() {}
    ~scene() = default;

    // prevent copy
    scene(const scene&) = delete;
    void operator=(const scene&) = delete;

    void update();

    void load(const std::string& file);
    void load_scene(const std::string& file);

private:

    // NOTE: Could do faster implementation by sorting sparse map itself
    // would require rework of sparse map adding indirection and pooling
    // to prevent entire structure being copied during swap operations
    ecs::container::sparse_set<> m_entities;
}; /* end of class scene */

} /* end of namespace tavern::system */

#endif /* end of define guard SCENE_H */
