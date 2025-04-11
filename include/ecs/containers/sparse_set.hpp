#ifndef SPARSE_SET_HPP
#define SPARSE_SET_HPP

#include <cassert>
#include <cstring>
#include <cstdint>
#include <vector>
#include <algorithm>

// slighlty hacky, better way?
#define SPARSE_SET_MAX_SIZE_DEFAULT UINT32_MAX - 1
#define SPARSE_SET_PAGE_SIZE_DEFAULT 2048

namespace ecs::container {

template<uint32_t max_size = SPARSE_SET_MAX_SIZE_DEFAULT, uint32_t page_size = SPARSE_SET_PAGE_SIZE_DEFAULT>
class sparse_set
{
public:
    static_assert(max_size < UINT32_MAX && max_size > 0, "max size must be greater than 0 and less than UINT32_MAX");

    typedef std::vector<uint32_t>::const_iterator const_iterator;
    typedef std::vector<uint32_t>::iterator iterator;

    sparse_set() {
        // init sparse array for all pages, accounting for integer division rounding down
        m_sparse = new uint32_t*[page_count()]{nullptr};
    }

    ~sparse_set() {
        delete_pages();
        delete[] m_sparse;
        m_sparse = nullptr;
    }

    // disable copy
    sparse_set(const sparse_set&) = delete;
    void operator=(const sparse_set&) = delete;

    [[nodiscard]] constexpr uint32_t tombstone() const {
        return max_size + 1;
    }

    uint32_t size() const {
        return m_dense.size();
    }

    // returns dense index of x or tombstone if not found
    uint32_t find(const uint32_t x) const {
        return sparse_peek(x);
    }

    bool exists(const uint32_t x) const {
        return sparse_peek(x) != tombstone();
    }

    void emplace(const uint32_t x)
    {
        uint32_t& dense_index = sparse_get(x);

        if (dense_index != tombstone())
            return;

        dense_index = m_dense.size();
        m_dense.push_back(x);
    }

    void remove(const uint32_t x)
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

    const uint32_t& operator[](const uint32_t index) const {
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

    void swap(const uint32_t a, const uint32_t b) {
        uint32_t a_dense = sparse_peek(a);
        uint32_t b_dense = sparse_peek(b);

        if (a_dense == tombstone() || b_dense == tombstone())
            return;

        dense_swap(a_dense, b_dense);
    }

private:

    uint32_t sparse_peek(const uint32_t x) const
    {
        //assert(x < max_size && "Index cannot exceed max size!");
        const uint32_t page_index = x / page_size;

        if (x >= max_size || !page_exists(page_index))
            return tombstone();

        return m_sparse[page_index][x % page_size];
    }

    uint32_t& sparse_get(const uint32_t x)
    {
        assert(x < max_size && "Index cannot exceed max size!");
        const uint32_t page_index = x / page_size;

        if (!page_exists(page_index)) {
            // NOTE: Ends up with un-used memory on last page if max size is not multiple of page size, up to page_size - 1 wasted space
            m_sparse[page_index] = new uint32_t[page_size];

            // set all values to tombstone
            // cant use memset as cannot garuantee entity_type is sizeof(byte) as in default case
            for (size_t i = 0; i < page_size; ++i)
                m_sparse[page_index][i] = tombstone();
        }

        return m_sparse[page_index][x % page_size];
    }

    [[nodiscard]] constexpr uint32_t page_count() const {
        return (max_size / page_size) + 1;
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

    uint32_t** m_sparse;
    std::vector<uint32_t> m_dense;
}; /* class sparse_set */

} /* namespace ecs::container */

#endif /* end of include guard: SPARSE_SET_HPP */
