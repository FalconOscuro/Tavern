#include "tavern/core/scene.h"

#include <boost/log/trivial.hpp>

#include "tavern/components/component_yaml_conversions.hpp"
#include "tavern/components/entity_name.h"

namespace tavern {

template <typename T>
inline void write_all_components(ryml::NodeRef& root, const ecs::registry& reg, const std::unordered_map<ecs::entity_type, size_t>& eid_map)
{
    const auto* pool = reg.try_get_pool<T>();

    // nothing to serialize
    if (!pool)
        return;

    for (auto it = pool->begin(); it != pool->end(); ++it)
    {
        auto found = eid_map.find(it->id);
        if (found == eid_map.end()) {
            BOOST_LOG_TRIVIAL(error) << "Serialization failure, could not find attached entity to component!!";
            continue;
        }

        const size_t parent_id = found->second;
        const size_t id = root.num_children();
        auto id_str = root.to_arena(id);

        ryml::NodeRef doc = root.append_child();
        doc |= ryml::DOC;

        doc << it->component;

        doc.set_val_anchor(id_str);

        ryml::NodeRef parent = root[parent_id];
        ryml::NodeRef comp_entry = parent["Components"].append_child();

        comp_entry.set_key(component::get_type_tag<T>());
        comp_entry.set_val(id_str);
    }
}

void scene::save(const std::string& file_name) const
{
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    tree.add_tag_directive("%TAG " TAVERN_TAG_DIRECTIVE " tag:tavern,2025:");

    root |= ryml::STREAM;

    {
        ryml::NodeRef header = root.append_child();
        header |= ryml::DOCMAP;

        header.set_val_tag(TAVERN_TAG_DIRECTIVE "header");

        header.append_child() << ryml::key("Version") << 1;
    }

    std::unordered_map<ecs::entity_type, size_t> eid_map;

    // each object is an individual document within yaml file
    for (auto it = m_registry.entities_begin(); it != m_registry.entities_end(); ++it)
    {
        const size_t id = root.num_children();
        eid_map.emplace(std::make_pair(*it, id)); 

        ryml::NodeRef entity = root.append_child();
        entity |= ryml::DOCMAP;

        // name component special case
        const auto* name = m_registry.try_get<component::entity_name>(*it);
        if (name)
            entity.append_child() << ryml::key("Name") << name->name;

        ryml::NodeRef components = entity.append_child();
        components.set_key("Components");
        components |= ryml::MAP | ryml::BLOCK;

        entity.set_val_tag(TAVERN_TAG_DIRECTIVE "entity");
        entity.set_val_anchor(entity.to_arena(id));
    }

    write_all_components<component::camera>(root, m_registry, eid_map);
    write_all_components<component::transform>(root, m_registry, eid_map);

    // write to file
    FILE* file = fopen(file_name.c_str(), "w");
    
    if (!file) {
        // throw exception?
        BOOST_LOG_TRIVIAL(error) << "Failed trying to save scene to file " << file_name << " on fopen";
        return;
    }

    //tree.resolve_tags();
    ryml::emit_yaml(tree, file);

    BOOST_LOG_TRIVIAL(trace) << "Scene saved to " << file_name;

    fclose(file);

    tree.clear_arena();
}

} /* end of namespace tavern */
