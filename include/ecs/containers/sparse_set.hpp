#ifndef SPARSE_SET_HPP
#define SPARSE_SET_HPP

#include <cassert>
#include <cstring>
#include <cstdint>
#include <vector>
#include <algorithm>

#include "../entity/entity.h"

#ifndef SPARSE_SET_PAGE_SIZE
#define SPARSE_SET_PAGE_SIZE 2048
#endif

namespace ecs::container {

class sparse_set
{
public:

    typedef std::vector<entity_type>::const_iterator const_iterator;
    typedef std::vector<entity_type>::iterator iterator;

    sparse_set() {
        // init sparse array for all pages, accounting for integer division rounding down
        m_sparse = new entity_type*[page_count()]{nullptr};
    }

    ~sparse_set() {
        delete_pages();
        delete[] m_sparse;
        m_sparse = nullptr;
    }

    // disable copy
    sparse_set(const sparse_set&) = delete;
    void operator=(const sparse_set&) = delete;

    [[nodiscard]] static constexpr entity_type tombstone() {
        return ENTITY_TOMBSTONE;
    }

    [[nodiscard]] uint32_t size() const {
        return m_dense.size();
    }

    // returns dense index of x or tombstone if not found
    [[nodiscard]] uint32_t find(const entity_type x) const {
        return sparse_peek(x);
    }

    [[nodiscard]] bool exists(const entity_type x) const {
        return sparse_peek(x) != tombstone();
    }

    void emplace(const entity_type x)
    {
        uint32_t& dense_index = sparse_get(x);

        if (dense_index != tombstone())
            return;

        dense_index = m_dense.size();
        m_dense.push_back(x);
    }

    void remove(const entity_type x)
    {
        uint32_t dense_index = sparse_peek(x);

        if (dense_index == tombstone())
            return;

        if (dense_index < size() - 1)
        {
            std::iter_swap(m_dense.begin() + dense_index, m_dense.end() - 1);
            sparse_get(m_dense[dense_index]) = dense_index;
        }

        m_dense.pop_back();
        sparse_get(x) = tombstone();
    }

    // removes without swapping to maintain list order
    // do not use if order is not important as this is slower
    iterator remove_inplace(const iterator& x)
    {
        uint32_t dense_index = sparse_peek(*x);

        if (dense_index == tombstone())
            return m_dense.end();

        sparse_get(*x) = tombstone();

        auto ret_it = m_dense.begin() + dense_index;
        ret_it = m_dense.erase(ret_it);

        // decrement preceeding elements in dense map
        for (auto it = ret_it; it != m_dense.end(); ++it)
            --sparse_get(*it);

        return ret_it;
    }

    const_iterator remove_inplace(const const_iterator& x)
    {
        uint32_t dense_index = sparse_peek(*x);

        if (dense_index == tombstone())
            return m_dense.cend();

        auto ret_it = m_dense.cbegin() + dense_index;
        ret_it = m_dense.erase(ret_it);

        // decrement preceeding elements in dense map
        for (auto it = ret_it; it != m_dense.cend(); ++it)
            --sparse_get(*it);

        return ret_it;
    }

    void clear()
    {
        m_dense.clear();
        delete_pages();
    }

    // indexing dense array
    entity_type operator[](const uint32_t index) const {
        return m_dense[index];
    }

    const_iterator cbegin() const {
        return m_dense.cbegin();
    }

    const_iterator begin() const {
        return cbegin();
    }

    iterator begin() {
        return m_dense.begin();
    }

    const_iterator cend() const {
        return m_dense.cend();
    }

    const_iterator end() const {
        return m_dense.end();
    }

    iterator end() {
        return m_dense.end();
    }

    // swap positions of two entities in dense array
    void swap(const entity_type a, const entity_type b)
    {
        uint32_t a_dense = sparse_peek(a);
        uint32_t b_dense = sparse_peek(b);

        if (a_dense == tombstone() || b_dense == tombstone())
            return;

        dense_swap(a_dense, b_dense);
    }

private:

    uint32_t sparse_peek(const entity_type x) const
    {
        //assert(x < max_size && "Index cannot exceed max size!");
        const uint32_t page_index = x / SPARSE_SET_PAGE_SIZE;

        if (x >= ENTITY_TOMBSTONE || !page_exists(page_index))
            return tombstone();

        return m_sparse[page_index][x % SPARSE_SET_PAGE_SIZE];
    }

    uint32_t& sparse_get(const entity_type x)
    {
        assert(x < ENTITY_TOMBSTONE && "Index cannot exceed max size!");
        const uint32_t page_index = x / SPARSE_SET_PAGE_SIZE;

        if (!page_exists(page_index)) {
            // NOTE: Ends up with un-used memory on last page if max size is not multiple of page size, up to page_size - 1 wasted space
            m_sparse[page_index] = new uint32_t[SPARSE_SET_PAGE_SIZE];

            // set all values to tombstone
            // cant use memset as cannot garuantee entity_type is sizeof(byte) as in default case
            std::fill_n(m_sparse[page_index], SPARSE_SET_PAGE_SIZE, tombstone());
        }

        return m_sparse[page_index][x % SPARSE_SET_PAGE_SIZE];
    }

    [[nodiscard]] constexpr uint32_t page_count() const {
        return (ENTITY_TOMBSTONE / SPARSE_SET_PAGE_SIZE) + 1;
    }

    void dense_swap(const uint32_t a, const uint32_t b)
    {
        if (a == b)
            return;

        std::iter_swap(m_dense.begin() + a, m_dense.begin() + b);

        // post swap dense still points to same position in swap
        sparse_get(m_dense[a]) = a;
        sparse_get(m_dense[b]) = b;
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
    std::vector<entity_type> m_dense;
}; /* class sparse_set */

} /* namespace ecs::container */

#endif /* end of include guard: SPARSE_SET_HPP */
