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
class resource_ptr;

template <typename Resource>
class resource_type_manager
{
public:
    typedef Resource resource_type;
    using resource_handle = resource_ptr<resource_type>;

    resource_type_manager()
    {}

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

// basic wrapper around std::shared_ptr for reference management,
// a bit hacky, but need to store resource id_name
// Could use name LUT?
template <typename Resource>
class resource_ptr
{
public:
    resource_ptr() = default;
    resource_ptr(const resource_ptr& r):
        m_ptr(r.m_ptr), m_name(r.m_name)
    {}

    ~resource_ptr() = default;

    resource_ptr& operator=(const resource_ptr& r) {
        m_ptr = r.m_ptr;
        m_name = r.m_name;

        return *this;
    }

    // reset to be nullptr
    void reset() noexcept {
        m_ptr.reset();
        m_name = "";
    }

    const std::string& get_name() const {
        return m_name;
    }

    Resource* get() const {
        return m_ptr.get();
    }

    Resource& operator*() const noexcept {
        return *m_ptr;
    }

    Resource* operator->() const noexcept {
        return m_ptr.get();
    }

    operator bool() const {
        return (bool)m_ptr;
    }

private:

    resource_ptr(const std::shared_ptr<Resource>& ptr, const std::string& name):
        m_ptr(ptr), m_name(ptr ? name : "") // empty name if nullptr
    {}

    std::shared_ptr<Resource> m_ptr;
    // copy slow...
    std::string m_name = "";

    friend class resource_type_manager<Resource>;
}; /* end of class resource_ptr */

} /* end of namespace tavern::resource */

#endif /* end of define guard RESOURCE_TYPE_MANAGER_HPP */
