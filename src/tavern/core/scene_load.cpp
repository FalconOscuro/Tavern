#include "tavern/core/scene.h"

#include <unordered_map>

#include <boost/log/trivial.hpp>

#include <ryml.hpp>

#include "tavern/components/component_yaml_conversions.hpp"
#include "tavern/resource/util/file.hpp"

template <>
struct std::hash<c4::csubstr>
{
    std::size_t operator()(const c4::csubstr& val) const
    {
        const std::string_view view = std::string_view(val.data(), val.size());

        return std::hash<std::string_view>{}(view);
    }
};

namespace tavern {

bool scene::load(const std::string& file_name)
{
    BOOST_LOG_TRIVIAL(info) << "Loading scene from " << file_name;

    size_t size;
    char* raw = resource::utility::read_file(file_name.c_str(), size);
    (void)size;

    if (raw == nullptr) {
        BOOST_LOG_TRIVIAL(error) << "Could not read file: " << file_name;
        return false;
    }

    const ryml::Tree tree = ryml::parse_in_place(raw);
    const ryml::ConstNodeRef root = tree.crootref();

    if (!root.is_stream()) {
        BOOST_LOG_TRIVIAL(error) << "Unexpected file layout: Expected document stream, loading failed!";
        delete[] raw;
        return false;
    }

    ryml::ConstNodeRef header = root.first_child();
    if (header.val_tag() != TAVERN_TAG_DIRECTIVE "header") {
        BOOST_LOG_TRIVIAL(error) << "Could not find header: should be first in doc stream!";
        delete[] raw;
        return false;
    }

    int file_ver;
    header["Version"] >> file_ver;
    if (file_ver != SAVE_FILE_VERSION) {
        BOOST_LOG_TRIVIAL(error) << "Incompatible save file version, got " << file_ver << " expected " << SAVE_FILE_VERSION;
        return false;
    }

    std::unordered_map<c4::csubstr, ecs::entity_type> eid_map;

    for (auto it = ++root.begin(); it != root.end(); ++it)
    {
        auto node = *it;

        if (node.val_tag() != TAVERN_TAG_DIRECTIVE "entity")
            continue;

        BOOST_LOG_TRIVIAL(trace) << node.val_anchor();

        if (eid_map.count(node.val_anchor()))
        {
            BOOST_LOG_TRIVIAL(warning) << "Found duplicate eid whilst loading file, skipping";
            continue;
        }

        const ecs::entity_type entity = m_registry.create();

        eid_map.emplace(std::make_pair(node.val_anchor(), entity));

        if (node.has_child("Name")) {
            std::string name;
            node["Name"] >> name;

            m_registry.emplace<component::entity_name>(entity, name);
        }
    }

    delete[] raw;
    return true;
}

} /* end of namespace tavern */
