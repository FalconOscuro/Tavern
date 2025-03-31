#ifndef TPK_MOUNT_H
#define TPK_MOUNT_H

#include <memory.h>
#include <unordered_map>

#include "imount.h"
#include "tpk/tpk.h"

namespace tavern::file {

struct file_tree_node
{
    using dir_map = std::unordered_map<std::string_view, std::unique_ptr<file_tree_node>>;

    file_tree_node(const tpk::file_type type = tpk::DIRECTORY);
    ~file_tree_node();

    const file_tree_node* find_node(const std::string_view path) const;

    tpk::file_type type;

    union {
        const tpk::file_node* file;

        struct
        {
            dir_map* entry_map;

            const tpk::directory_entry* entries;
        } directory; /* end of struct  */
    } data;
};

class tpk_mount final : public imount
{
public:
    tpk_mount(const std::string_view path);

    ~tpk_mount();

    bool has_file(const std::string_view path) const override;
    std::unique_ptr<ifile> load_file(const std::string_view path) const override;

    bool valid() const override;

    const std::string_view get_identifier() const override;

    const tpk::header& header() const {
        return m_header;
    }

private:

    file_tree_node m_root;

    // node 0 is reserved for root directory
    bool parse_directory_tree(const size_t index, file_tree_node* node);

    size_t get_data_start_pos() const;

    void force_invalidate() {
        m_header.sig[0] = '\0';
    }

    tpk::header m_header;

    const tpk::file_node* m_file_nodes = nullptr;
};

} /* namespace tavern::file */

#endif /* end of include guard: TPK_MOUNT_H */
