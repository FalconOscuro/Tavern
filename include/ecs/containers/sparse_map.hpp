#ifndef SPARSE_MAP_HPP
#define SPARSE_MAP_HPP

#include <cstdint>
#include <vector>
#include <cassert>
#include <algorithm>

#include "amorphic_vector.hpp"

namespace ecs::container {

class sparse_map_base
{
public:
    virtual ~sparse_map_base() {};

    virtual void remove(const uint32_t entity) = 0;
};

template<typename mapped_type>
struct sparse_map_node
{
    sparse_map_node();
    sparse_map_node(uint32_t id, const mapped_type& component):
        id(id), component(component)
    {}

    ~sparse_map_node() = default;

    uint32_t id;
    mapped_type component;
}; /* end of struct sparse_map_node */

template<typename mapped_type, uint32_t max_size = UINT32_MAX - 1, uint32_t page_size = 2048>
class sparse_map : public sparse_map_base
{
public:
    // need pair with id for iterating
    typedef sparse_map_node<mapped_type> node_type;

    typedef typename std::vector<node_type>::const_iterator const_iterator;
    typedef typename std::vector<node_type>::iterator iterator;

    static_assert(max_size < UINT32_MAX && max_size > 0, "max size must be greater than 0 and less than UINT32_MAX");

    sparse_map() {
        m_sparse = new uint32_t*[page_count()]{nullptr};
    }

    ~sparse_map() {
        delete_pages();
        delete[] m_sparse;
    }

    // disable copy
    sparse_map(const sparse_map&) = delete;
    void operator=(const sparse_map&) = delete;

    [[nodiscard]] constexpr uint32_t tombstone() const {
        return max_size + 1;
    }

    uint32_t size() const {
        return m_dense.size();
    }

    bool exists(const uint32_t x) const {
        return sparse_peek(x) != tombstone();
    }

    template<typename... Args>
    mapped_type& emplace(const uint32_t x, Args&&... args)
    {
        uint32_t& dense_index = sparse_get(x);

        if (dense_index == tombstone()) {
            dense_index = size();
            m_dense.emplace_back(node_type(x, mapped_type(std::forward<Args>(args)...)));
        }

        else
            m_dense[dense_index].component = mapped_type(std::forward<Args>(args)...);

        return m_dense[dense_index].component;
    }

    void remove(const uint32_t x) override
    {
        uint32_t dense_index = sparse_peek(x);

        if (dense_index == tombstone())
            return;

        if (dense_index < size() - 1)
        {
            std::iter_swap(m_dense.begin() + sparse_peek(x), m_dense.end() - 1);
            sparse_get(m_dense[dense_index].id) = dense_index;
        }

        m_dense.pop_back();
        sparse_get(x) = tombstone();
    }

    void clear() {
        delete_pages();
        m_dense.clear();
    }

    // allocate if not found
    mapped_type& get(const uint32_t x)
    {
        uint32_t dense_index = sparse_peek(x);

        if (dense_index != tombstone())
            return m_dense[dense_index].component;

        return emplace<>(x);
    }

    const mapped_type* try_get(const uint32_t entity) const
    {
        uint32_t dense_index = sparse_peek(entity);

        if (dense_index == tombstone())
            return nullptr;

        return &(m_dense[dense_index].component);
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
        return cend();
    }

    iterator end() {
        return m_dense.end();
    }

    // sorting?

private:

    uint32_t sparse_peek(const uint32_t x) const
    {
        assert(x < max_size && "Index cannot exceed max size!");
        const uint32_t page_index = x / page_size;

        if (!page_exists(page_index))
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
        sparse_get(m_dense[a].id) = a;
        sparse_get(m_dense[b].id) = b;
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
    std::vector<node_type> m_dense;
}; /* class sparse_map */

} /* namespace ecs::container */

#endif /* end of include guard: SPARSE_MAP_HPP */
