#ifndef SPARSE_MAP_HPP
#define SPARSE_MAP_HPP

#include <cstdint>
#include <cassert>
#include <algorithm>

#include "amorphic_vector.hpp"
#include "component_container.hpp"
#include "sparse_set.hpp"
#include "../entity/entity.h"

#ifndef SPARSE_MAP_PAGE_SIZE
#define SPARSE_MAP_PAGE_SIZE SPARSE_SET_PAGE_SIZE
#endif

namespace ecs::container {

// remove templating dependency into wrapper
class sparse_map final
{
public:
    // need pair with id for iterating
    typedef typename amorphic_vec::const_iterator const_iterator;
    typedef typename amorphic_vec::iterator iterator;

    template<typename T>
    sparse_map(std::in_place_type_t<T> type):
        m_component_type_info(type), m_dense(std::in_place_type<component_container<T>>)
    {
        static_assert(offsetof(component_container<T>, entity_id) == 0, "entity ID must be at pointed type address!");

        init_sparse();
    }

    // USE WITH CAUTION, has no checking to enforce pointed address is ID!
    // Could add flag to type info?
    sparse_map(const internal::type_info& type_info):
        m_component_type_info(type_info), m_dense(type_info)
    {
        init_sparse();
    }

    ~sparse_map() {
        delete_pages();
        delete[] m_sparse;
    }

    // disable copy
    sparse_map(const sparse_map&) = delete;
    void operator=(const sparse_map&) = delete;

    const internal::type_info& get_type_info() const {
        return m_component_type_info;
    }

    const internal::type_info& get_container_type_info() const {
        return m_dense.get_type_info();
    }

    // USE WITH CAUTION
    amorphic_vec& get_dense() {
        return m_dense;
    }

    const amorphic_vec& get_dense() const {
        return m_dense;
    }

    [[nodiscard]] static constexpr entity_type tombstone() {
        return ENTITY_TOMBSTONE;
    }

    [[nodiscard]] uint32_t size() const {
        return m_dense.size();
    }

    [[nodiscard]] bool exists(const entity_type eid) const {
        return sparse_peek(eid) != tombstone();
    }

    void remove(const entity_type eid)
    {
        uint32_t dense_index = sparse_peek(eid);

        if (dense_index == tombstone())
            return;

        if (dense_index < size() - 1)
        {
            amorphic_iter_swap(m_dense.begin() + sparse_peek(eid), m_dense.end() - 1);

            // Potentially unsafe, need to add garuantee that member at pointed address is always the eid
            sparse_get(*reinterpret_cast<entity_type*>(m_dense[dense_index])) = dense_index;
        }

        m_dense.pop_back();
        sparse_get(eid) = tombstone();
    }

    void clear() {
        delete_pages();
        m_dense.clear();
    }
    
    // NOTE: Return only does default init of component data,
    // only explicitly sets eid
    // for create, do check exists first?
    void* get(const entity_type eid)
    {
        uint32_t& dense_index = sparse_get(eid);

        if (dense_index != tombstone())
            return m_dense[dense_index];

        dense_index = m_dense.size();
        void* array_ptr = m_dense.push_back();

        // init
        m_dense.get_type_info().constructor(array_ptr);
        
        // assign eid
        *reinterpret_cast<entity_type*>(array_ptr) = eid;
        return array_ptr;
    }

    void* try_get(const entity_type eid)
    {
        uint32_t dense_index = sparse_peek(eid);

        if (dense_index == tombstone())
            return nullptr;

        return m_dense[dense_index];
    }

    const void* try_get(const entity_type eid) const
    {
        uint32_t dense_index = sparse_peek(eid);

        if (dense_index == tombstone())
            return nullptr;

        return m_dense[dense_index];
    }

    const_iterator cbegin() const {
        return m_dense.cbegin();
    }

    const_iterator begin() const {
        return m_dense.cbegin();
    }

    iterator begin() {
        return m_dense.begin();
    }

    const_iterator cend() const {
        return m_dense.cend();
    }

    const_iterator end() const {
        return m_dense.cend();
    }

    iterator end() {
        return m_dense.end();
    }

    // operator[]?

    // sorting?

private:

    inline void init_sparse()
    {
        m_sparse = new entity_type*[page_count()]{nullptr};
        assert(m_dense.get_type_info().is_valid_component_container && "Type is not flagged as a valid component container, check component_container<T> if using native type");
    }

    uint32_t sparse_peek(const entity_type eid) const
    {
        assert(eid < ENTITY_TOMBSTONE && "Index cannot exceed max size!");
        const uint32_t page_index = eid / SPARSE_MAP_PAGE_SIZE;

        if (!page_exists(page_index))
            return tombstone();

        return m_sparse[page_index][eid % SPARSE_MAP_PAGE_SIZE];
    }

    uint32_t& sparse_get(const entity_type eid)
    {
        assert(eid < ENTITY_TOMBSTONE && "Index cannot exceed max size!");
        const uint32_t page_index = eid / SPARSE_MAP_PAGE_SIZE;

        if (!page_exists(page_index)) {
            // NOTE: Ends up with un-used memory on last page if max size is not multiple of page size, up to page_size - 1 wasted space
            m_sparse[page_index] = new entity_type[SPARSE_MAP_PAGE_SIZE];

            std::fill_n(m_sparse[page_index], SPARSE_MAP_PAGE_SIZE, tombstone());
        }

        return m_sparse[page_index][eid % SPARSE_MAP_PAGE_SIZE];
    }

    [[nodiscard]] constexpr uint32_t page_count() const {
        return (ENTITY_TOMBSTONE / SPARSE_MAP_PAGE_SIZE) + 1;
    }

    void dense_swap(const uint32_t a, const uint32_t b)
    {
        if (a == b)
            return;

        amorphic_iter_swap(m_dense.begin() + a, m_dense.begin() + b);

        // post swap dense still points to same position in swap
        sparse_get(*reinterpret_cast<const entity_type*>(m_dense[a])) = a;
        sparse_get(*reinterpret_cast<const entity_type*>(m_dense[b])) = b;
    }

    bool page_exists(const uint32_t page_num) const {
        return m_sparse[page_num] != nullptr;
    }

    void delete_pages() {
        for (uint32_t i = 0; i < page_count(); i++) {
            delete[] m_sparse[i];
            m_sparse[i] = nullptr;
        }
    }

    entity_type** m_sparse;

    internal::type_info m_component_type_info;

    // temporary whilst changing over
    amorphic_vec m_dense;
    //wrapped_amorphic_vec<node_type> m_dense;
}; /* class sparse_map */

} /* namespace ecs::container */

#endif /* end of include guard: SPARSE_MAP_HPP */
