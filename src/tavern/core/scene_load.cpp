#include "tavern/core/scene.h"

#include <chrono>
#include <unordered_map>

#include <boost/log/trivial.hpp>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include "tavern/components/component_yaml_conversions.hpp"
#include "tavern/resource/util/file.hpp"
#include "tavern/core/file_system.h"

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
    const auto start = std::chrono::high_resolution_clock::now();
    BOOST_LOG_TRIVIAL(info) << "Loading scene from " << file_name;

    size_t size;
    auto raw = resource::utility::read_file(file_name.c_str(), size);
    (void)size;

    if (raw == nullptr) {
        BOOST_LOG_TRIVIAL(error) << "Could not read file: " << file_name;
        return false;
    }

    const ryml::Tree tree = ryml::parse_in_place(raw.get());
    const ryml::ConstNodeRef root = tree.crootref();

    if (!root.is_stream()) {
        BOOST_LOG_TRIVIAL(error) << "Unexpected file layout: Expected document stream, loading failed!";
        return false;
    }

    // NOTE: non static positions?
    // read header
    {
        ryml::ConstNodeRef header = root.first_child();
        if (header.val_tag() != TAVERN_TAG_DIRECTIVE "header") {
            BOOST_LOG_TRIVIAL(error) << "Could not find header: should be first in doc stream!";
            return false;
        }

        int file_ver;
        header["version"] >> file_ver;
        if (file_ver != SAVE_FILE_VERSION) {
            BOOST_LOG_TRIVIAL(error) << "Incompatible save file version, got " << file_ver << " expected " << SAVE_FILE_VERSION;
            return false;
        }
    }

    // read file_system state
    {
        ryml::ConstNodeRef file_sys_info = root.child(1);
        auto& file_sys = file_system::singleton();

        if (file_sys_info.val_tag() != TAVERN_TAG_DIRECTIVE "file_system") {
            BOOST_LOG_TRIVIAL(error) << "Could not find file_system state info: should be second in doc stream!";
            return false;
        }

        ryml::ConstNodeRef mounts = file_sys_info["mounts"];

        for (auto it = mounts.begin(); it != mounts.end(); ++it)
        {
            file::mount_path mount_path;
            *it >> mount_path;

            if (file_sys.is_mounted(std::string(mount_path.get_identifier()))) {
                BOOST_LOG_TRIVIAL(trace) << mount_path << " appears already mounted, skipping";
                continue;
            }

            file_sys.mount(mount_path);
        }
    }

    std::unordered_map<c4::csubstr, ecs::entity_type> eid_map;

    for (auto it = ++root.begin(); it != root.end(); ++it)
    {
        auto node = *it;

        if (node.val_tag() != TAVERN_TAG_DIRECTIVE "entity")
            continue;

        ecs::entity_type entity;
        using namespace component;

        if (!eid_map.count(node.val_anchor())) {
            entity = m_registry.create();
            eid_map.emplace(std::make_pair(node.val_anchor(), entity));
        }

        else
            entity = eid_map[node.val_anchor()];

        if (node.has_child("name")) {
            std::string name;
            node["name"] >> name;

            m_registry.emplace<entity_name>(entity, name);
        }

        // load individual components
        ryml::ConstNodeRef components = node["components"];

        // Currently using std::string for has_child calls, temporary measure as std::string_view is unsupported

        // transform
        if (components.has_child(component_type_info<transform>::c4_name()))
        {
            auto& transf = *m_registry.emplace<transform>(entity);

            size_t transf_doc_id;
            components[component_type_info<transform>::c4_name()] >> transf_doc_id;
            ryml::ConstNodeRef transf_node = root.child(transf_doc_id);

            transf_node >> transf;

            if (transf.parent != ecs::entity_type(-1))
            {
                ecs::entity_type parent_id;
                c4::csubstr parent_id_substr = transf_node["parent"].val();

                if (!eid_map.count(parent_id_substr)) {
                    parent_id = m_registry.create();
                    eid_map.emplace(std::make_pair(parent_id_substr, parent_id));
                }

                else
                    parent_id = eid_map[parent_id_substr];

                transf.parent = parent_id;
            }
        }

        // camera
        if (components.has_child(component_type_info<camera>::c4_name()))
        {
            auto& cam = *m_registry.emplace<camera>(entity);
            size_t doc_id;
            components[component_type_info<camera>::c4_name()] >> doc_id;

            root.child(doc_id) >> cam;
        }

        if (components.has_child(component_type_info<render_mesh>::c4_name()))
        {
            auto& mesh = *m_registry.emplace<render_mesh>(entity);
            size_t doc_id;
            components[component_type_info<render_mesh>::c4_name()] >> doc_id;

            root.child(doc_id) >> mesh;
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    BOOST_LOG_TRIVIAL(trace) << "Successfully loaded scene in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.f << " seconds";

    return true;
}

} /* end of namespace tavern */
