#ifndef RESOURCE_TYPE_MANAGER_HPP
#define RESOURCE_TYPE_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <boost/log/trivial.hpp>

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
    typedef std::shared_ptr<resource_type> resource_handle;

    resource_type_manager()
    {}

    resource_handle load(const std::string& path)
    {
        const std::size_t hash = get_hash(path);
        resource_handle res_handle = try_get(hash);

        if (res_handle)
            return res_handle;

        res_handle = resource_handle(load_new(path), resource_deleter<Resource>(&m_loaded, hash));

        if (res_handle != nullptr)
            m_loaded.emplace(hash, res_handle);

        else
            BOOST_LOG_TRIVIAL(error) << "Failed to load resource: " << path;

        return res_handle;
    }

    resource_handle register_new(const Resource& x, const std::string& name)
    {
        const std::size_t hash = get_hash(name);
        resource_handle res_handle = try_get(hash);

        // throw warning?
        if (res_handle)
            return res_handle;

        res_handle = resource_handle(new Resource(x), resource_deleter<Resource>(&m_loaded, hash));
        return res_handle;
    }

    bool is_loaded(const std::string& name) const {
        const std::size_t hash = get_hash(name);
        auto res = m_loaded.find(hash);

        return res != m_loaded.end() && !res->second.expired();
    }

protected:
    typedef resource_type* resource_ptr;

    // Could be useful to specify memory location for pooling
    virtual resource_ptr load_new(const std::string& path) = 0;

private:

    resource_handle try_get(const std::size_t hash) const {
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
