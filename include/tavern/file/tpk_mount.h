#ifndef TPK_MOUNT_H
#define TPK_MOUNT_H

#include <cstdint>
#include <unordered_map>

#include "imount.h"
#include "physical_file.h"
#include "tpk_file.h"

#define TPK_VERSION 0
#define TPK_SIG "TPK"

namespace tavern {

struct tpk_header
{
    // signature for file verification should always be "TPK"
    char sig[3];

    // tpk format version
    uint16_t fmt_version;

    // version number for tpk contents
    uint32_t dat_version;

    // number of files stored in tpk
    uint32_t num_entries;

    char name[24];
    char author[24];

    // requirements?
    // tags?
};

class tpk_mount : public mount
{
public:
    tpk_mount(const std::string& path);

    ~tpk_mount() = default;

    bool has_file(const std::string& path) const override;
    std::unique_ptr<file> load_file(const std::string& path) override;

    bool valid() const override;

private:

    std::unordered_map<std::string, tpk_file_info> m_file_table;

    physical_file m_file;
    tpk_header m_header;
};

} /* namespace tavern */

#endif /* end of include guard: TPK_MOUNT_H */
