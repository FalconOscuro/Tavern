#ifndef TPK_H
#define TPK_H

#include <cstdint>
#include <cstddef>

#define TPK_VERSION 1
#define TPK_SIG "TPK"

namespace tavern::file::tpk {

struct header
{
    static constexpr auto SIG_SIZE = sizeof(TPK_SIG);
    // signature for file verification should always be "TPK"
    char sig[SIG_SIZE] = TPK_SIG;

    // could be 2 bytes as w/ fmt_version
    // version number for tpk contents
    uint32_t dat_version = 0;

    // number of file nodes stored in tpk
    uint32_t num_nodes = 0;

    // swap w/ dat_version?
    // tpk format version
    uint16_t fmt_version = TPK_VERSION;

    static constexpr size_t MAX_NAME_LEN = 32;
    static constexpr size_t MAX_AUTHOR_LEN = 26;

    char name[MAX_NAME_LEN]{'\0'};
    char author[MAX_AUTHOR_LEN]{'\0'};

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
    uint64_t start = 0;

    // size of file
    uint32_t size = 0;

    file_type type = UNKNOWN;
}; /* end of struct tpk_file_node */

struct directory
{
    size_t num_entries = 0;
}; /* end of struct directory */

struct directory_entry
{
    size_t node_index = 0;

    uint8_t name_len = 0;

    // 119 scaled down from 128 to remove padding
    static constexpr size_t MAX_NAME_LEN = 119;
    char name[MAX_NAME_LEN]{'\0'};
}; /* end of struct directory_entry */


} /* end of namespace tavern::file::tpk */

#endif /* end of define guard TPK_H */
