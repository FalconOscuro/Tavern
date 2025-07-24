#include "file/tpk_mount.h"

#include <cstring>

#include <boost/log/trivial.hpp>

#include "file/tpk_dir.h"
#include "file/tpk_file.h"
#include "file/physical_file.h"

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
    // at endpoint or fully traversed path
    if (type == tpk::FILE || path.empty())
        return path.empty() ? this : nullptr;

    const size_t split = path.find_first_of('/');
    const std::string_view sub_path = path.substr(0, split);

    const auto found = data.directory.entry_map->find(sub_path);

    return found == data.directory.entry_map->end() ? nullptr :
        found->second->find_node(split == path.npos ? path.substr(0,0) : path.substr(split));
}

tpk_mount::tpk_mount(const std::string_view path):
    imount(path)
{
    physical_file file(path);

    if(!file.open())
        return;

    if (!(file.get_data(&m_header) == 1 && valid()))
        return;

    tpk::file_node* nodes = new tpk::file_node[m_header.num_nodes];
    // store as read only
    m_file_nodes = nodes;

    const size_t root_index = m_header.num_nodes - 1;
    const size_t bytes_read_expected = sizeof(tpk::file_node) * m_header.num_nodes;
    const size_t bytes_read = file.get_str(reinterpret_cast<char*>(nodes), bytes_read_expected);

    if (!m_header.num_nodes) {
        BOOST_LOG_TRIVIAL(error) << "TPK does not appear to contain any file nodes!";
        force_invalidate();
    }

    else if (bytes_read != bytes_read_expected) {
        BOOST_LOG_TRIVIAL(error) << "Failed to read all file nodes, "
            << "expected: " << m_header.num_nodes << '(' << bytes_read_expected << "b), "
            << "read: " << bytes_read / sizeof(tpk::file_node) << '(' << bytes_read << "b)";
        force_invalidate();
    }

    else if (m_file_nodes[root_index].type != tpk::file_type::DIRECTORY) {
        BOOST_LOG_TRIVIAL(error) << "Expected position for root directory file node was not of type directory! found: " << m_file_nodes[root_index].type;
        force_invalidate();
    }

    else if (!parse_directory_tree(root_index, &m_root)) {
        BOOST_LOG_TRIVIAL(error) << "Critical failure whilst parsing TPK root directory";
        force_invalidate();
    }
}

tpk_mount::~tpk_mount() {
    delete[] m_file_nodes;
}

bool tpk_mount::has_file(const std::string_view path) const
{
    const file_tree_node* node = m_root.find_node(path);

    return node != nullptr && node->type == tpk::file_type::FILE;
}

file_handle tpk_mount::load_file(const std::string_view path) const
{
    if (!valid())
        return nullptr;

    const file_tree_node* node = m_root.find_node(path);

    if (node == nullptr || node->type != tpk::FILE)
        return nullptr;

    return file_handle(new tpk_file(get_path(), mount_path(get_identifier(), path), node->data.file, get_data_start_pos()));
}

bool tpk_mount::has_dir(const std::string_view path) const
{
    const file_tree_node* node = m_root.find_node(path);

    return node != nullptr && node->type == tpk::file_type::DIRECTORY;
}

dir_handle tpk_mount::load_dir(const std::string_view path) const
{
    if (!valid())
        return nullptr;

    const file_tree_node* node = m_root.find_node(path);

    if (node == nullptr || node->type != tpk::DIRECTORY)
        return nullptr;

    return dir_handle(new tpk_dir(mount_path(get_identifier(), path), this, node->data.directory.entry_map));
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
    if (index >= m_header.num_nodes) {
        BOOST_LOG_TRIVIAL(error) << "File node index out of range! index: " << index << ", max_index: " << m_header.num_nodes - 1;
        return false;
    }

    tpk_file dir_file = tpk_file(get_path(), mount_path(), m_file_nodes + index, get_data_start_pos());

    if (!dir_file.open()) {
        BOOST_LOG_TRIVIAL(error) << "Failed to open tpk directory, file index: " << index << ", at: " << m_file_nodes[index].start + get_data_start_pos();
        return false;
    }

    tpk::directory dir_info;

    // failed to read directory file header
    if (!(dir_file.get_data(&dir_info) == 1 && dir_info.num_entries)) {
        BOOST_LOG_TRIVIAL(error) << "Failed to read directory header, found " << dir_info.num_entries << " entries";
        return false;
    }

    BOOST_LOG_TRIVIAL(trace) << dir_info.num_entries;

    tpk::directory_entry* entries = new tpk::directory_entry[dir_info.num_entries];
    node->data.directory.entries = entries;

    // failed to read all entries in file
    if (dir_file.get_data(entries, dir_info.num_entries) != dir_info.num_entries) {
        BOOST_LOG_TRIVIAL(error) << "Failed to read all entires, expected " << dir_info.num_entries;
        return false;
    }

    for (size_t i = 0; i < dir_info.num_entries; ++i)
    {
        const tpk::directory_entry* entry = entries + i;

        // skip if:
        // - node index out of range
        // - no name
        if (entry->node_index >= m_header.num_nodes || entry->name_len == 0) {
            BOOST_LOG_TRIVIAL(error) << "(" << entry->node_index << " >= " << m_header.num_nodes << ") || (" << (unsigned short int)entry->name_len << " == 0)";
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
