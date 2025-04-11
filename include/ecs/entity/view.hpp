#ifndef VIEW_HPP
#define VIEW_HPP

#include <tuple>
#include <type_traits>

#include "ecs/containers/sparse_map.hpp"
#include "ecs/core/typeinfo.hpp"

namespace ecs {

template <typename... Component>
class view_iterator
{
    template <typename...> friend class view;
public:

    ~view_iterator() = default;

    template <typename T>
    T& get() {

        auto pool = std::get<core::type_index<T, Component...>::index>(m_pools);

        return pool->get(*m_iterator);
    }

    template <typename T>
    const T& get() const {
        auto pool = std::get<core::type_index<T, Component...>::index>(m_pools);

        return pool->get(*m_iterator);
    }

    // preincrement
    view_iterator& operator++() {
        ++m_iterator;
        return *this;
    }

    // postincrement
    view_iterator operator++(int) {
        view_iterator old = *this;
        m_iterator++;
        return old;
    }

    view_iterator& operator--() {
        --m_iterator;
        return *this;
    }

    view_iterator operator--(int) {
        view_iterator old = *this;
        m_iterator--;
        return old;
    }

    bool operator==(const view_iterator& vi) const {
        return m_iterator == vi.m_iterator;
    }

    bool operator!=(const view_iterator& vi) const {
        return m_iterator != vi.m_iterator;
    }

    uint32_t operator*() const {
        return *m_iterator;
    }

private:

    view_iterator(const std::vector<uint32_t>::iterator& iterator, const std::tuple<container::sparse_map<Component>* ...>& pools):
        m_iterator(iterator), m_pools(pools)
    {}

    std::vector<uint32_t>::iterator m_iterator;
    
    std::tuple<container::sparse_map<Component>* ...> m_pools;

}; /* end of class view_iterator */

// does this need to be specific separate class?
template <typename... Component>
class const_view_iterator
{
    template <typename...> friend class view;
public:

    ~const_view_iterator() = default;

    template <typename T>
    const T& get() {
        auto pool = std::get<core::type_index<T, Component...>::index>(m_pools);

        // technically discards constness, should be fine???
        return pool->get(*m_iterator);
    }

    // preincrement
    const_view_iterator& operator++() {
        ++m_iterator;
        return *this;
    }

    // postincrement
    const_view_iterator operator++(int) {
        const_view_iterator old = *this;
        m_iterator++;
        return old;
    }

    const_view_iterator& operator--() {
        --m_iterator;
        return *this;
    }

    const_view_iterator operator--(int) {
        view_iterator old = *this;
        m_iterator--;
        return old;
    }

    bool operator==(const const_view_iterator& vi) const {
        return m_iterator == vi.m_iterator;
    }

    bool operator!=(const const_view_iterator& vi) const {
        return m_iterator != vi.m_iterator;
    }

    const uint32_t& operator*() const {
        return *m_iterator;
    }

private:

    const_view_iterator(const std::vector<uint32_t>::const_iterator& iterator, const std::tuple<container::sparse_map<Component>* ...>& pools):
        m_iterator(iterator), m_pools(pools)
    {}

    std::vector<uint32_t>::const_iterator m_iterator;
    
    std::tuple<container::sparse_map<Component>* ...> m_pools;

}; /* end of class const_view_iterator */

// WARNING: mutlithreading undefined
template<typename... Component>
class view
{
    template <typename Type>
    using pool_type = container::sparse_map<Type>;
public:

    typedef view_iterator<Component...> iterator;
    typedef const_view_iterator<Component...> const_iterator;

    view(pool_type<Component>* ... pool):
        m_search_pools(pool ...)
    {
        find_smallest();
    }

    ~view() = default;

    // call every frame, otherwise list doesn't update leading to undefined behaviour
    void update() {
        m_entities.clear();
        find_smallest();
    }

    uint32_t size() {
        return m_entities.size();
    }

    const_iterator cbegin() const {
        return const_iterator(m_entities.cbegin(), m_search_pools);
    }

    const_iterator begin() const {
        return cbegin();
    }

    iterator begin() {
        return iterator(m_entities.begin(), m_search_pools);
    }

    const_iterator cend() const {
        return const_iterator(m_entities.cend());
    }

    const_iterator end() const {
        return cend();
    }

    iterator end() {
        return iterator(m_entities.end(), m_search_pools);
    }

private:

    // hacky way to iterate tuples to find smallest pool
    template <uint32_t I = 0>
    constexpr typename std::enable_if<I < sizeof...(Component), void>::type find_smallest(uint32_t smallest_size = UINT32_MAX, uint32_t smallest_id = 0) {
        
        uint32_t size = std::get<I>(m_search_pools)->size();

        if (size < smallest_size) {
            smallest_size = size;
            smallest_id = I;
        }

        find_smallest<I + 1>(smallest_size, smallest_id);
    }

    template <uint32_t I = 0>
    constexpr typename std::enable_if<I == sizeof...(Component), void>::type find_smallest(uint32_t smallest_size = UINT32_MAX, uint32_t smallest_id = 0) {

        // either something went wrong or one pool is empty, so no combinations possible
        if (smallest_size == UINT32_MAX || smallest_size == 0)
            return;

        populate_entities(smallest_id);
    }

    template <uint32_t I = 0>
    constexpr typename std::enable_if<I < sizeof...(Component), void>::type populate_entities(const uint32_t smallest_id) {

        if (I != smallest_id)
            return populate_entities<I + 1>(smallest_id);

        auto pool = std::get<I>(m_search_pools);

        for (auto it = pool->begin(); it != pool->end(); it++) {
            if (check_exists<I>(it->id))
                m_entities.emplace_back(it->id);
        }
    }

    template <uint32_t I = 0>
    constexpr typename std::enable_if<I == sizeof...(Component), void>::type populate_entities(const uint32_t smallest_id) {
        (void)smallest_id;
        assert(false && "Critical failure, reached bottom of view but expected to find pool!");
    }

    template <uint32_t source_pool, uint32_t I = 0>
    constexpr typename std::enable_if<I < sizeof...(Component) && I != source_pool, bool>::type check_exists(const uint32_t entity) const {
        return std::get<I>(m_search_pools)->exists(entity) && check_exists<source_pool, I + 1>(entity);
    }

    template <uint32_t source_pool, uint32_t I = 0>
    constexpr typename std::enable_if<I == sizeof...(Component), bool>::type check_exists(const uint32_t entity) const {
        (void)entity;
        return true;
    }

    template <uint32_t source_pool, uint32_t I = 0>
    constexpr typename std::enable_if<I < sizeof...(Component) && I == source_pool, bool>::type check_exists(const uint32_t entity) const {
        return check_exists<source_pool, I + 1>(entity);
    }

    std::tuple<pool_type<Component>* ...> m_search_pools;
    std::vector<uint32_t> m_entities;

}; /* end of class view */

} /* end of namespace ecs */

#endif /* end of define guard VIEW_HPP */
