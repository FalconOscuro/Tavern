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
    typedef std::shared_ptr<Resource> resource_ptr;

    resource_type_manager()
    {}

    resource_ptr load(const std::string& path)
    {
        const std::size_t hash = std::hash<std::string>{}(path);
        auto res = m_loaded.find(hash);
        resource_ptr res_ptr;

        // WARNING: Not threadsafe
        if (res != m_loaded.end()) {
            res_ptr = res->second.lock();

            if (res_ptr != nullptr)
                return res_ptr;
        }

        res_ptr = resource_ptr(load_new(path), resource_deleter<Resource>(&m_loaded, hash));

        if (res_ptr != nullptr)
            m_loaded.emplace(hash, res_ptr);

        else
            BOOST_LOG_TRIVIAL(error) << "Failed to load resource: " << path;

        return res_ptr;
    }

    void clear() {
    }

protected:

    // Could be useful to specify memory location for pooling
    virtual Resource* load_new(const std::string& path) = 0;

private:

    std::unordered_map<std::size_t, std::weak_ptr<Resource>> m_loaded;
}; /* end of class resource_type_manager<Resource> */

} /* end of namespace tavern::resource */

#endif /* end of define guard RESOURCE_TYPE_MANAGER_HPP */
