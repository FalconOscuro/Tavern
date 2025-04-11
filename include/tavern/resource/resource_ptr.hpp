#ifndef RESOURCE_PTR_H
#define RESOURCE_PTR_H

#include <memory>
#include <string>

#include <file/path.h>

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
        m_ptr(r.m_ptr), m_path(r.m_path)
    {}

    ~resource_ptr() = default;

    resource_ptr& operator=(const resource_ptr& r) {
        m_ptr = r.m_ptr;
        m_path = r.m_path;

        return *this;
    }

    // reset to be nullptr
    void reset() noexcept {
        m_ptr.reset();
        m_path = file::mount_path();
    }

    const file::mount_path& get_path() const {
        return m_path;
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

    resource_ptr(const std::shared_ptr<Resource>& ptr, const file::mount_path& path):
        m_ptr(ptr)
    {
        if (ptr)
            m_path = path;
    }

    std::shared_ptr<Resource> m_ptr;
    // copy slow...
    file::mount_path m_path;

    friend class resource_type_manager<Resource>;
}; /* end of class resource_ptr */

} /* namespace tavern::resource */

#endif /* end of include guard: RESOURCE_PTR_H */
