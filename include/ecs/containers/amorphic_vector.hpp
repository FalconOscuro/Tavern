#ifndef AMORPHIC_VECTOR_HPP
#define AMORPHIC_VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <utility>

#include "../entity/type.hpp"

namespace ecs::container {

class const_amorphic_vec_iterator
{
public:
    const_amorphic_vec_iterator(const const_amorphic_vec_iterator&);
    //const_amorphic_vec_iterator(const amorphic_vec_iterator&);

    const_amorphic_vec_iterator& operator++();
    const_amorphic_vec_iterator& operator--();

    const_amorphic_vec_iterator operator++(int);
    const_amorphic_vec_iterator operator--(int);

    bool operator ==(const const_amorphic_vec_iterator&) const;
    bool operator !=(const const_amorphic_vec_iterator&) const;

}; /* end of class const_amorphic_vec_iterator */

class amorphic_vec_iterator
{
public:

    amorphic_vec_iterator& operator++();
    amorphic_vec_iterator& operator--();

    amorphic_vec_iterator operator++(int);
    amorphic_vec_iterator operator--(int);

    bool operator==(const amorphic_vec_iterator&) const;
    bool operator!=(const amorphic_vec_iterator&) const;

    bool operator==(const const_amorphic_vec_iterator&) const;
    bool operator!=(const const_amorphic_vec_iterator&) const;

    operator const_amorphic_vec_iterator() const;

}; /* end of class amorphic_vec_iterator */

class amorphic_vec
{
    using byte = unsigned char;

public:

    template <typename T>
    amorphic_vec(std::in_place_type_t<T> type): amorphic_vec(internal::type_info(type))
    {}

    amorphic_vec(const internal::type_info& type_info): m_type_info(type_info)
    {
        m_data = malloc(type_info.size * m_capacity);
    }

    ~amorphic_vec()
    {
        clear();
        free(m_data);
    }

    inline const internal::type_info& get_type_info() const {
        return m_type_info;
    }

    void* operator[](size_t i) {
        return reinterpret_cast<byte*>(m_data) + (i * m_type_info.size);
    }

    const void* operator[](size_t i) const {
        return reinterpret_cast<const byte*>(m_data) + (i * m_type_info.size);
    }

    // only increases size
    void reserve(size_t capacity)
    {
        // space already reserved
        if (m_capacity >= capacity)
            return;

        change_size(capacity);
    }

    void shrink_to_fit()
    {
        const size_t capacity_used = m_size + 1;

        // greater than should be irellavant here, but just in case
        if (capacity_used < m_capacity)
            change_size(capacity_used);
    }

    void clear()
    {
        for (size_t i = 0; i < m_size; ++i)
            m_type_info.destructor(reinterpret_cast<byte*>(m_data) + (m_type_info.size * i));

        m_size = 0;
    }

    size_t size() const {
        return m_size;
    }

    size_t max_size() const {
        // max size = maximum number of calculateable bytes
        return (std::numeric_limits<size_t>::max() / m_type_info.size) - 1;
    }

    size_t capacity() const {
        return m_capacity;
    }

    void* front() {
        return m_data;
    }

    const void* front() const {
        return m_data;
    }

    void* back() {
        return reinterpret_cast<byte*>(m_data) + (m_type_info.size * (m_size - 1));
    }

    const void* back() const {
        return reinterpret_cast<const byte*>(m_data) + (m_type_info.size * (m_size - 1));
    }

    void* data() {
        return m_data;
    }

    const void* data() const {
        return m_data;
    }

private:

    inline size_t calculate_new_capacity() const {
        return m_capacity * 2;
    }

    void change_size(size_t capacity) {
        // ensure capacity never hits 0
        capacity = std::max<size_t>(capacity, 1);

        // need to allocate 1 beyond for use as end iterator
        void* new_vec = realloc(m_data, m_type_info.size * (capacity + 1));

        // verify completion
        if (!new_vec) {
            // THROW
            return;
        }

        m_capacity = capacity;
        m_data = new_vec;
    }

    const internal::type_info m_type_info;

    void* m_data;

    size_t m_size = 0;

    // Ensure never reaches 0
    size_t m_capacity = 1;

    // cache
    //const size_t m_max_size;

}; /* end of class amorphic_vec */

} /* end of namespace ecs::container */

#endif /* end of define guard AMORPHIC_VECTOR_HPP */
