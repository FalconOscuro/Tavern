#ifndef RESOURCE_TYPE_MANAGER_HPP
#define RESOURCE_TYPE_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <boost/log/trivial.hpp>

#include "resource_ptr.hpp"

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

    resource_type_manager() {
        // None reserved for nullptr, hack
        m_loaded.emplace(get_hash("None"), std::weak_ptr<resource_type>());
    }

    resource_handle load(const std::string& path)
    {
        const std::size_t hash = get_hash(path);
        resource_shared_ptr res_ptr = try_get(hash);

        if (res_ptr)
            return resource_handle(res_ptr, path);

        res_ptr = resource_shared_ptr(load_new(path), resource_deleter<Resource>(&m_loaded, hash));

        // issues if failed to load resource with deleter triggering?
        if (res_ptr)
            m_loaded.emplace(hash, res_ptr);

        else
            BOOST_LOG_TRIVIAL(error) << "Failed to load resource: " << path;

        return resource_handle(res_ptr, path);
    }

    template <typename... Args>
    resource_handle register_new(const std::string& name, Args&&... args)
    {
        const std::size_t hash = get_hash(name);
        resource_shared_ptr res_ptr = try_get(hash);

        if (res_ptr)
            return resource_handle(res_ptr, name);

        res_ptr = resource_shared_ptr(new Resource(std::forward<Args>(args)...), resource_deleter<Resource>(&m_loaded, hash));

        return resource_handle(res_ptr, name);
    }

    bool is_loaded(const std::string& name) const {
        const std::size_t hash = get_hash(name);
        auto res = m_loaded.find(hash);

        return res != m_loaded.end() && !res->second.expired();
    }

protected:

    // Could be useful to specify memory location for pooling
    virtual resource_type* load_new(const std::string& path) = 0;

private:
    typedef std::shared_ptr<resource_type> resource_shared_ptr;

    resource_shared_ptr try_get(const std::size_t hash) const {
        auto res = m_loaded.find(hash);
        return res != m_loaded.end() ? res->second.lock() : nullptr;
    }

    static std::size_t get_hash(const std::string& name) {
        return std::hash<std::string>{}(name);
    }

    std::unordered_map<std::size_t, std::weak_ptr<Resource>> m_loaded;
}; /* end of class resource_type_manager<Resource> */

} /* end of namespace tavern::resource */

#endif /* end of define guard RESOURCE_TYPE_MANAGER_HPP */
