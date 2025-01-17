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
    typedef std::unordered_map<std::string, std::weak_ptr<Resource>> container;
public:
    resource_deleter(container* mgr, const std::string& name):
        Name(name), m_container(mgr)
    {}

    resource_deleter(const resource_deleter& d):
        Name(d.Name), m_container(d.m_container)
    {}

    void operator()(Resource* res) {
        m_container->erase(Name);
        delete res;
    }

    // Note: Copy string will cause slowdown when passing shared pointers
    const std::string Name;

private:

    container* m_container;

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
        auto res = m_loaded.find(path);

        if (!(res == m_loaded.end() || res->second.expired()))
            return res->second.lock();

        resource_ptr res_ptr = resource_ptr(load_new(path), resource_deleter<Resource>(&m_loaded, path));

        if (res_ptr != nullptr)
            m_loaded.emplace(path, res_ptr);

        else
            BOOST_LOG_TRIVIAL(error) << "Failed to load resource: " << path;

        return res_ptr;
    }

protected:

    // Could be useful to specify memory location for pooling
    virtual Resource* load_new(const std::string& path) = 0;

private:

    std::unordered_map<std::string, std::weak_ptr<Resource>> m_loaded;
}; /* end of class resource_type_manager<Resource> */

} /* end of namespace tavern::resource */

#endif /* end of define guard RESOURCE_TYPE_MANAGER_HPP */
