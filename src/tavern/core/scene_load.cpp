#include "tavern/core/scene.h"

#include <unordered_map>

#include <boost/log/trivial.hpp>

#include <ryml.hpp>

#include "tavern/components/component_yaml_conversions.hpp"
#include "tavern/resource/util/file.hpp"

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

    for (auto it = ++root.begin(); it != root.end(); ++it)
    {
        auto node = *it;

        if (node.val_tag() != TAVERN_TAG_DIRECTIVE "entity")
            continue;

        // need to get eid
        m_registry.create();
    }

    delete[] raw;
    return true;
}

} /* end of namespace tavern */
