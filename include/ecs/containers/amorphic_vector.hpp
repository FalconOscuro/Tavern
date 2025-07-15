#ifndef AMORPHIC_VECTOR_HPP
#define AMORPHIC_VECTOR_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

#include "../entity/type.hpp"

namespace ecs::container {

template<bool IsConst>
class amorphic_vec_iterator_t final
{
    friend class amorphic_vec;

    using ptr_type = std::conditional_t<IsConst, const void*, void*>;
    using byte_ptr_type = std::conditional_t<IsConst, const unsigned char*, unsigned char*>;

public:
    // metaprogramming to prevent conversion back from const, should be impossible regardless due to using directives
    // static asserts?
    template<bool IsConstOther, class = std::enable_if_t<IsConst && !IsConstOther>>
    amorphic_vec_iterator_t(const amorphic_vec_iterator_t<IsConstOther>& other):
        m_data(other.m_data), m_type_info(other.m_type_info)
    {
        static_assert_prevent_const_regression<IsConstOther>();
    }
    
    template<bool IsConstOther, class = std::enable_if_t<IsConst && !IsConstOther>>
    amorphic_vec_iterator_t& operator=(const amorphic_vec_iterator_t<IsConstOther>& other)
    {
        static_assert_prevent_const_regression<IsConstOther>();
        assert_same_type(other);

        m_data = other.m_data;
        m_type_info = other.m_type_info;

        return *this;
    }

    inline ptr_type data() const {
        return m_data;
    }

    inline const internal::type_info& get_type_info() const {
        return m_type_info;
    }

    ptr_type operator[](const int n) const {
        return increment_ptr(n);
    }

    amorphic_vec_iterator_t& operator++()
    {
        m_data = increment_ptr();
        return *this;
    }

    amorphic_vec_iterator_t& operator--()
    {
        m_data = decrement_ptr();
        return *this;
    }

    amorphic_vec_iterator_t operator++(int)
    {
        amorphic_vec_iterator_t i = amorphic_vec_iterator_t(*this);
        m_data = increment_ptr();
        return i;
    }

    amorphic_vec_iterator_t operator--(int)
    {
        amorphic_vec_iterator_t i = amorphic_vec_iterator_t(*this);
        m_data = decrement_ptr();
        return i;
    }

    amorphic_vec_iterator_t operator+(int n) const {
        return amorphic_vec_iterator_t(m_type_info, increment_ptr(n));
    }

    amorphic_vec_iterator_t operator-(int n) const {
        return amorphic_vec_iterator_t(m_type_info, decrement_ptr(n));
    }

    amorphic_vec_iterator_t& operator+=(int n)
    {
        m_data = increment_ptr(n);
        return *this;
    }

    amorphic_vec_iterator_t& operator-=(int n)
    {
        m_data = decrement_ptr(n);
        return *this;
    }

    template<bool IsConstOther>
    bool operator==(const amorphic_vec_iterator_t<IsConstOther>& rhs) const
    {
        assert_same_type(rhs);
        return m_data == rhs.m_data;
    }

    template<bool IsConstOther>
    bool operator!=(const amorphic_vec_iterator_t<IsConstOther>& rhs) const
    {
        assert_same_type(rhs);
        return m_data != rhs.m_data || m_type_info != rhs.m_type_info;
    }

    template<bool IsConstOther>
    bool operator<(const amorphic_vec_iterator_t<IsConstOther>& rhs) const
    {
        assert_same_type(rhs);
        return m_data < rhs.m_data;
    }

    template<bool IsConstOther>
    bool operator<=(const amorphic_vec_iterator_t<IsConstOther>& rhs) const
    {
        assert_same_type(rhs);
        return m_data <= rhs.m_data;
    }

    template<bool IsConstOther>
    bool operator>(const amorphic_vec_iterator_t<IsConstOther>& rhs) const
    {
        assert_same_type(rhs);
        return m_data > rhs.m_data;
    }

    template<bool IsConstOther>
    bool operator>=(const amorphic_vec_iterator_t<IsConstOther>& rhs) const
    {
        assert_same_type(rhs);
        return m_data >= rhs.m_data;
    }

private:

    template<bool IsConstOther>
    inline void assert_same_type(const amorphic_vec_iterator_t<IsConstOther>& other) const {
        assert(m_type_info == other.m_type_info && "Attempted conversion between amorphic iterators of differing base types!");
        // use as exception instead?
    }

    template<bool IsConstOther>
    inline constexpr void static_assert_prevent_const_regression() const {
        static_assert(!IsConst || IsConst, "Detected const regression!");
    }

    amorphic_vec_iterator_t(const internal::type_info& type_info, ptr_type data):
        m_type_info(type_info), m_data(data)
    {}

    // could reuse with negative
    inline ptr_type increment_ptr(const int i = 1) const {
        return reinterpret_cast<byte_ptr_type>(m_data) + (m_type_info.size * i);
    }

    inline ptr_type decrement_ptr(const int i = 1) const {
        return reinterpret_cast<byte_ptr_type>(m_data) - (m_type_info.size * i);
    }

    const internal::type_info m_type_info;
    ptr_type m_data;

}; /* end of class amorphic_vec_iterator_t */

typedef amorphic_vec_iterator_t<false> amorphic_vec_iterator;
typedef amorphic_vec_iterator_t<true> const_amorphic_vec_iterator;

inline void amorphic_iter_swap(const amorphic_vec_iterator& a, const amorphic_vec_iterator& b)
{
    const internal::type_info& type_info = a.get_type_info();

    assert(a.get_type_info() == b.get_type_info());

    // Don't like array alloc use each time here, could maintain small pool somehow, and resize when needed?

    unsigned char* temp = new unsigned char[type_info.size];

    memcpy(temp, a.data(), type_info.size);
    memcpy(a.data(), b.data(), type_info.size);
    memcpy(b.data(), temp, type_info.size);

    delete[] temp;
}

// Vector for storing data-types, potentially unknown at compile time
// Modelled to fit structure and behaviour of std::vector as closely as possible, although some features are currently missing
class amorphic_vec final
{
    using byte = unsigned char;

public:

    // Un-needed?
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

    void pop_back()
    {
        if (m_size == 0)
            return;

        m_type_info.destructor(get_index_ptr(m_size--));
    }

    // WARNING POTENTIALLY UNSAFE, USE WITH CAUTION!!
    // could make private and only allow access via wrapper?
    void push_back(const void* value_ptr)
    {
        const size_t new_idx = m_size;
        change_size(new_idx + 1);

        // quit here, potential use later for wrappers to allocate without immediate copy?
        if (value_ptr == nullptr)
            return;

        memcpy(get_index_ptr(new_idx), value_ptr, m_type_info.size);
    }

    inline const internal::type_info& get_type_info() const {
        return m_type_info;
    }

    // at(size_t i) w/ bounds checking?

    void* operator[](size_t i) {
        return get_index_ptr(i);
    }

    const void* operator[](size_t i) const {
        return get_index_ptr(i);
    }

    // only increases size
    void reserve(size_t capacity)
    {
        // space already reserved
        if (m_capacity >= capacity)
            return;

        change_capacity(capacity);
    }

    void shrink_to_fit()
    {
        // greater than should be irellavant here, but just in case
        if (m_size < m_capacity)
            change_capacity(m_size);
    }

    void clear()
    {
        // could just increment byte offset?
        for (size_t i = 0; i < m_size; ++i)
            m_type_info.destructor(get_index_ptr(i));

        change_size(0);
    }

    size_t size() const {
        return m_size;
    }

    size_t max_size() const {
        // max size = maximum number of calculatable bytes
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
        return get_index_ptr(m_size == 0 ? 0 : m_size - 1);
    }

    const void* back() const {
        return get_index_ptr(m_size == 0 ? 0 : m_size - 1);
    }

    void* data() {
        return m_data;
    }

    const void* data() const {
        return m_data;
    }

    amorphic_vec_iterator begin() {
        return amorphic_vec_iterator(m_type_info, m_data);
    }

    const_amorphic_vec_iterator begin() const {
        return const_amorphic_vec_iterator(m_type_info, m_data);
    }

    const_amorphic_vec_iterator cbegin() const {
        return const_amorphic_vec_iterator(m_type_info, m_data);
    }

    amorphic_vec_iterator end() {
        return amorphic_vec_iterator(m_type_info, get_index_ptr(m_size));
    }

    const_amorphic_vec_iterator end() const {
        return const_amorphic_vec_iterator(m_type_info, get_index_ptr(m_size));
    }

    const_amorphic_vec_iterator cend() const {
        return const_amorphic_vec_iterator(m_type_info, get_index_ptr(m_size));
    }

private:

    inline void* get_index_ptr(const size_t i) const {
        return reinterpret_cast<byte*>(m_data) + (m_type_info.size * i);
    }

    inline size_t calculate_new_capacity() const {
        return m_capacity * 2;
    }

    void change_size(size_t new_size)
    {
        if (new_size > m_capacity)
        {
            size_t new_capacity = m_capacity;

            // double till can fit new size, overflow?
            // Could do max check instead of loop, but shouldn't cause noticeable difference, max 31 its because doubling
            while((new_capacity *= 2) < new_size);

            change_capacity(new_capacity);
        }

        m_size = new_size;
    }

    void change_capacity(size_t new_capacity)
    {
        // ensure capacity never hits 0
        new_capacity = std::max<size_t>(new_capacity, 1);

        if (new_capacity > max_size())
            throw std::length_error("Attempted to change amorphic vec capacity beyond maximum size!");

        void* new_vec = realloc(m_data, m_type_info.size * new_capacity);

        // verify completion
        if (!new_vec) {
            // THROW
            return;
        }

        m_capacity = new_capacity;
        m_data = new_vec;
    }

    const internal::type_info m_type_info;

     void* m_data;

    size_t m_size = 0;

    // Ensure never reaches 0
    // Not for maintaining end iterator as valid memory, just ensures at least 1 data member can be allocated before re-allocation is needed
    size_t m_capacity = 1;

    // cache
    //const size_t m_max_size;

}; /* end of class amorphic_vec */

} /* end of namespace ecs::container */

#endif /* end of define guard AMORPHIC_VECTOR_HPP */
