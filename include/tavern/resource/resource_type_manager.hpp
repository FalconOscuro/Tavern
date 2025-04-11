#ifndef RESOURCE_TYPE_MANAGER_HPP
#define RESOURCE_TYPE_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <boost/log/trivial.hpp>

#include "resource_ptr.hpp"
#include "tavern/core/file_system.h"

namespace tavern::resource {

template <typename Resource>
class resource_deleter
{
    typedef std::unordered_map<std::size_t, std::weak_ptr<Resource>> container;
public:
    resource_deleter(container* mgr, const std::size_t hash):
        m_container(mgr), m_hash(hash)
    {}

    resource_deleter(const resource_deleter& d):
        m_container(d.m_container), m_hash(d.m_hash)
    {}

    ~resource_deleter() = default;

    void operator()(Resource* res) {
        m_container->erase(m_hash);
        delete res;
    }

private:

    container* m_container;
    const std::size_t m_hash;

}; /* end of class resource_deleter */

template <typename Resource>
class resource_type_manager
{
public:
    typedef Resource resource_type;
    using resource_handle = resource_ptr<resource_type>;

    resource_type_manager() = default;

    resource_handle load(const file::mount_path& path)
    {
        const std::size_t hash = get_hash(path);
        resource_shared_ptr res_ptr = try_get(hash);

        if (res_ptr)
            return resource_handle(res_ptr, path);

        auto file = file_system::singleton().load_file(path);

        if (file && file->open())
            res_ptr = resource_shared_ptr(load_new(file.get()), resource_deleter<Resource>(&m_loaded, hash));

        else if (file)
            BOOST_LOG_TRIVIAL(error) << "Failed to open file: " << path;

        file.reset();

        // issues if failed to load resource with deleter triggering?
        if (res_ptr)
            m_loaded.emplace(hash, res_ptr);

        else
            BOOST_LOG_TRIVIAL(error) << "Failed to load resource: " << path;

        return resource_handle(res_ptr, path);
    }

    bool is_loaded(const file::mount_path& path) const {
        const std::size_t hash = get_hash(path);
        auto res = m_loaded.find(hash);

        return res != m_loaded.end() && !res->second.expired();
    }

protected:

    // Could be useful to specify memory location for pooling
    virtual resource_type* load_new(file::ifile* file) = 0;

private:
    typedef std::shared_ptr<resource_type> resource_shared_ptr;

    resource_shared_ptr try_get(const std::size_t hash) const {
        auto res = m_loaded.find(hash);
        return res != m_loaded.end() ? res->second.lock() : nullptr;
    }

    inline static std::size_t get_hash(const file::mount_path& path) {
        return std::hash<file::mount_path>{}(path);
    }

    std::unordered_map<std::size_t, std::weak_ptr<Resource>> m_loaded;
}; /* end of class resource_type_manager<Resource> */

} /* end of namespace tavern::resource */

#endif /* end of define guard RESOURCE_TYPE_MANAGER_HPP */
