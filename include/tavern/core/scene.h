#ifndef SCENE_H
#define SCENE_H

#include <ecs/ecs.h>

#define SAVE_FILE_VERSION 1

namespace tavern {

class scene
{
public:

    ~scene() = default;

    void shutdown();

    // prevent copy
    scene(const scene&) = delete;
    void operator=(const scene&) = delete;

    void update();

    [[nodiscard]] inline ecs::registry& get_registry() {
        return m_registry;
    }

    [[nodiscard]] static scene& singleton() {
        static scene instance;
        return instance;
    }

    void save(const std::string& file_name) const;
    bool load(const std::string& file_name);

    //void load(const std::string& file);
    //void load_scene(const std::string& file);

private:

    scene() = default;

    // NOTE: Could do faster implementation by sorting sparse map itself
    // would require rework of sparse map adding indirection and pooling
    // to prevent entire structure being copied during swap operations
    ecs::container::sparse_set<> m_entities;

    ecs::registry m_registry;
}; /* end of class scene */

} /* end of namespace tavern */

#endif /* end of define guard SCENE_H */
