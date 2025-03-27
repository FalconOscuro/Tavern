#include "tavern/file/tpk_mount.h"

#include <cstring>

#include <boost/log/trivial.hpp>

#include "tavern/file/tpk_file.h"
#include "tavern/file/physical_file.h"

namespace tavern::file {

file_tree_node::file_tree_node(const tpk::file_type type):
    type(type)
{
    switch (type)
    {
    case tpk::DIRECTORY:
        data.directory.entry_map = new dir_map();
        data.directory.entries = nullptr;
        break;

    case tpk::FILE:
        data.file = nullptr;
        break;

    default:
        assert(false && "Filetype unknown!!");
        break;
    }
}

file_tree_node::~file_tree_node()
{
    switch (type)
    {
    case tpk::DIRECTORY:
        delete data.directory.entry_map;
        delete[] data.directory.entries;
        break;

    default:
        break;
    }
}

const file_tree_node* file_tree_node::find_node(const std::string_view path) const
{
    // if file node, check if path fully traversed
    if (type == tpk::FILE)
        return path.empty() ? this : nullptr;

    const size_t split = path.find_first_of('/');
    const std::string_view sub_path = path.substr(0, split);

    const auto found = data.directory.entry_map->find(sub_path);

    return found == data.directory.entry_map->end() ? nullptr :
        found->second->find_node(path.substr(split));
}

tpk_mount::tpk_mount(const std::string& path):
    imount(path)
{
    physical_file file(path);

    if(!file.open())
        return;

    if (!(read_data(&m_header, &file) && valid()))
        return;

    tpk::file_node* nodes = new tpk::file_node[m_header.num_nodes];
    // store as read only
    m_file_nodes = nodes;

    const size_t root_index = m_header.num_nodes - 1;

    // could not read specified number of file nodes
    // or failed to parse directory tree
    if (!(read_data(nodes, &file, m_header.num_nodes) && m_file_nodes[root_index].type == tpk::file_type::DIRECTORY && parse_directory_tree(root_index, &m_root)))
    {
        BOOST_LOG_TRIVIAL(error) << "Parse failure, invalidating TPK";

        // hack to ensure valid returns false
        ++m_header.sig[0];
    }
}

tpk_mount::~tpk_mount() {
    delete[] m_file_nodes;
}

bool tpk_mount::has_file(const std::string& path) const {
    return m_root.find_node(path);
}

std::unique_ptr<ifile> tpk_mount::load_file(const std::string& path) const
{
    if (!valid())
        return nullptr;

    const file_tree_node* node = m_root.find_node(path);

    if (!node || node->type != tpk::FILE)
        return nullptr;

    return std::make_unique<tpk_file>(get_path(), mount_path(get_identifier(), path), node->data.file, get_data_start_pos());
}

bool tpk_mount::valid() const {
    return m_header.fmt_version == TPK_VERSION && strncmp(m_header.sig, TPK_SIG, strlen(TPK_SIG)) == 0 && m_header.num_nodes != 0;
}

const std::string_view tpk_mount::get_identifier() const
{
    if (!valid())
        return std::string_view();

    const std::string_view str_v = std::string_view(m_header.name, sizeof(m_header.name));
    const size_t end = str_v.find_first_of('\0');

    return str_v.substr(0, end);
}

bool tpk_mount::parse_directory_tree(const size_t index, file_tree_node* node)
{
    // index out of file node range
    if (index >= m_header.num_nodes)
        return false;

    tpk_file dir_file = tpk_file(get_path(), mount_path(), m_file_nodes + index, get_data_start_pos());

    if (!dir_file.open())
        return false;

    BOOST_LOG_TRIVIAL(trace) << "TPK dir file open success";

    tpk::directory dir_info;

    // failed to read directory file header
    if (!(read_data(&dir_info, &dir_file)) && dir_info.num_entries) {
        BOOST_LOG_TRIVIAL(error) << dir_info.num_entries;
        return false;
    }

    BOOST_LOG_TRIVIAL(trace) << "Dir header read success";

    tpk::directory_entry* entries = new tpk::directory_entry[dir_info.num_entries];
    node->data.directory.entries = entries;

    // failed to read all entries in file
    if (!read_data(entries, &dir_file, dir_info.num_entries)) {
        return false;
    }

    for (size_t i = 0; i < dir_info.num_entries; ++i)
    {
        const tpk::directory_entry* entry = entries + i;

        // skip if:
        // - node index out of range
        // - no name
        if (entry->node_index >= m_header.num_nodes || entry->name_len == 0) {
            BOOST_LOG_TRIVIAL(error) << "(" << entry->node_index << " >= " << m_header.num_nodes << ") || (" << entry->name_len << " == 0)";
            continue;
        }

        const tpk::file_node* file_node = m_file_nodes + entry->node_index;
        const std::string_view name = std::string_view(entry->name, entry->name_len);

        // file names cannot have '/'
        // avoid collisions
        if (name.find('/') != name.npos || node->data.directory.entry_map->count(name))
            continue;

        file_tree_node* branch = new file_tree_node(file_node->type);

        switch (file_node->type)
        {
        case tpk::DIRECTORY:
            BOOST_LOG_TRIVIAL(trace) << "subdir: " << name;
            // failed to parse directory
            if (!parse_directory_tree(entry->node_index, branch)) {
                BOOST_LOG_TRIVIAL(error) << "Failed to parse dir " << name;
                delete branch;
                continue;
            }
            break;

        case tpk::FILE:
            BOOST_LOG_TRIVIAL(trace) << "file: " << name;
            branch->data.file = file_node;
            break;

        default:
            BOOST_LOG_TRIVIAL(warning) << "Unkown file type: " << file_node->type;
            break;
        }

        BOOST_LOG_TRIVIAL(trace) << "Registered entry: " << name;
        node->data.directory.entry_map->emplace(std::make_pair(name, branch));
    }

    return true;
}

size_t tpk_mount::get_data_start_pos() const {
    return sizeof(tpk::header) + (sizeof(tpk::file_node) * m_header.num_nodes);
}

} /* namespace tavern::file */
