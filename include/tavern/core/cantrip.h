#ifndef TAVERN_CANTRIP_H
#define TAVERN_CANTRIP_H

#include <memory>
#include <unordered_map>

#include <cantrip/module/module.h>
#include <file/path.h>

namespace tavern::core {

class cantrip_modules
{
public:
    using module_ptr = std::shared_ptr<cantrip::module>;

    // need to pass directory, scan for cantrip.yml & read in module info
    module_ptr load_module(const file::mount_path& module_path);
    void unload_module(const std::string_view);

    void get_module();

    bool is_module_loaded() const;

private:

    std::unordered_map<std::string_view, std::shared_ptr<cantrip::module>> m_loaded_modules;
};

} /* namespace tavern::core */

#endif /* end of include guard: TAVERN_CANTRIP_H */
