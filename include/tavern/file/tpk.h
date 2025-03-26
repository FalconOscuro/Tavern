#ifndef TPK_H
#define TPK_H

#include <cstdint>
#include <cstddef>

// 119 scaled down from 128 to prevent padding
#define TPK_MAX_FILE_NAME_LEN 119

namespace tavern::file::tpk {

struct header
{
    // signature for file verification should always be "TPK"
    char sig[3];

    // tpk format version
    uint16_t fmt_version;

    // version number for tpk contents
    uint32_t dat_version;

    // number of file nodes stored in tpk
    uint32_t num_nodes;

    char name[32];
    char author[24];

    // requirements?
    // tags?
};

enum file_type {
    UNKNOWN,
    DIRECTORY,
    FILE
};

struct file_node
{
    // position of file node start
    uint64_t start;

    // size of file
    uint32_t size;

    file_type type;
}; /* end of struct tpk_file_node */

struct directory
{
    size_t num_entries;
}; /* end of struct directory */

struct directory_entry
{
    uint8_t name_len;
    char name[TPK_MAX_FILE_NAME_LEN];

    size_t node_index;
}; /* end of struct directory_entry */


} /* end of namespace tavern::file::tpk */

#endif /* end of define guard TPK_H */
