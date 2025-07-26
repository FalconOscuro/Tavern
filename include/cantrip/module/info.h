#ifndef CANTRIP_MODULE_INFO_H
#define CANTRIP_MODULE_INFO_H

#include <string>

namespace cantrip {

struct version
{
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
};

struct module_info
{
    std::string author;
    std::string email;

    std::string name;
    std::string description;

    version module_version;
    version cantrip_version;
};

} /* namespace cantrip */

#endif /* end of include guard: CANTRIP_MODULE_INFO_H */
