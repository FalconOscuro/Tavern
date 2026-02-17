#ifndef TAVERN_CANTRIP_H
#define TAVERN_CANTRIP_H

#include <memory>
#include <unordered_map>

#include <cantrip/module/module.h>
#include <file/path.h>

namespace tavern {

class cantrip_modules
{
public:
    using module_ptr = std::shared_ptr<cantrip::module>;
    using module_map = std::unordered_map<std::string, module_ptr>;

    ~cantrip_modules();

    bool init();
    void shutdown();

    // need to pass directory, scan for cantrip.yml & read in module info
    module_ptr load_module(const file::mount_path& module_path);
    void unload_module(const std::string& module_name);

    module_ptr get_module(const std::string_view module_name);

    bool is_module_loaded(const std::string_view module_name) const;

    void unload_all_modules();

    module_map::const_iterator begin() const {
        return m_loaded_modules.begin();
    }

    module_map::const_iterator end() const {
        return m_loaded_modules.end();
    }

    [[nodiscard]] static cantrip_modules& singleton() {
        static cantrip_modules instance;
        return instance;
    }

private:

    cantrip_modules() = default;

    // NOTE: Shared_ptr can prevent proper unloading!
    module_map m_loaded_modules;
};

} /* namespace tavern::core */

#endif /* end of include guard: TAVERN_CANTRIP_H */
