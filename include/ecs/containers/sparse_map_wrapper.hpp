#ifndef SPARSE_MAP_WRAPPER_H
#define SPARSE_MAP_WRAPPER_H

#include "amorphic_vector_wrapper.hpp"
#include "sparse_map.hpp"

namespace ecs::container {

// const...
template <typename component_type>
class wrapped_sparse_map final
{
public:

    typedef component_container<component_type> container_type;
    using wrapped_vec_type = wrapped_amorphic_vec<container_type>;

    typedef typename wrapped_vec_type::iterator iterator;
    typedef typename wrapped_vec_type::const_iterator const_iterator;

    wrapped_sparse_map(sparse_map* wrapped_map):
        m_wrapped_map(wrapped_map), m_wrapped_vec(&wrapped_map->get_dense())
    {
        assert(m_wrapped_map != nullptr);
        assert(core::type_info(std::in_place_type<container_type>) == m_wrapped_map->get_container_type_info());
    }

    ~wrapped_sparse_map() = default;
    wrapped_sparse_map(const wrapped_sparse_map&) = default;
    wrapped_sparse_map& operator=(const wrapped_sparse_map&) = default;

    const core::type_info& get_type_info() const {
        return m_wrapped_map->get_type_info();
    }

    const core::type_info& get_container_type_info() const {
        return m_wrapped_map->get_container_type_info();
    }

    [[nodiscard]] constexpr entity_type tombstone() const {
        return sparse_map::tombstone();
    }

    [[nodiscard]] uint32_t size() const {
        return m_wrapped_map->size();
    }

    [[nodiscard]] bool exists(const entity_type eid) const {
        return m_wrapped_map->exists(eid);
    }

    // would prefer more direct method?
    template<typename... Args>
    container_type& emplace(const entity_type eid, Args&&... args) const
    {
        container_type* container_ptr = reinterpret_cast<container_type*>(m_wrapped_map->get(eid));

        container_ptr->component = component_type(std::forward<Args>(args)...);

        return *container_ptr;
    }

    void remove(const entity_type eid) {
        m_wrapped_map->remove(eid);
    }

    void clear() {
        m_wrapped_map->clear();
    }

    container_type& get(const entity_type eid) {
        return *reinterpret_cast<container_type*>(m_wrapped_map->get(eid));
    }

    const container_type* try_get(const entity_type eid) const {
        return reinterpret_cast<const container_type*>(m_wrapped_map->try_get(eid));
    }

    const_iterator cbegin() const {
        return m_wrapped_vec.cbegin();
    }

    const_iterator begin() const {
        return m_wrapped_vec.cbegin();
    }

    iterator begin() {
        return m_wrapped_vec.begin();
    }

    const_iterator cend() const {
        return m_wrapped_vec.cend();
    }

    const_iterator end() const {
        return m_wrapped_vec.cend();
    }

    iterator end() {
        return m_wrapped_vec.end();
    }

private:

    sparse_map* m_wrapped_map;
    wrapped_vec_type m_wrapped_vec;
};

} /* namespace ecs::container */

#endif /* end of include guard: SPARSE_MAP_WRAPPER_H */
