#ifndef RESOURCE_PTR_H
#define RESOURCE_PTR_H

#include <memory>
#include <string>

namespace tavern::resource {

template<typename Resource>
class resource_type_manager;

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

    const std::string& get_name() const
    {
        // need static var as return by ref not by val
        static const std::string none_str = "None";
        return m_name.empty() ? none_str : m_name;
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

} /* namespace tavern::resource */

#endif /* end of include guard: RESOURCE_PTR_H */
