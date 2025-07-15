#ifndef AMORPHIC_VECTOR_WRAPPER_H
#define AMORPHIC_VECTOR_WRAPPER_H

#include "amorphic_vector.hpp"

namespace ecs::container {

template<typename T>
class wrapped_amorphic_vec;

template<typename T>
class wrapped_amorphic_vec_iterator final
{
    using is_const = std::is_const<T>;
    using base_it = std::conditional_t<is_const::value, const_amorphic_vec_iterator, amorphic_vec_iterator>;

    friend class wrapped_amorphic_vec<std::remove_cv_t<T>>;

public:

    wrapped_amorphic_vec_iterator(const wrapped_amorphic_vec_iterator&) = default;
    wrapped_amorphic_vec_iterator& operator=(const wrapped_amorphic_vec_iterator&) = default;

    // See amorphic_vec_iterator_t for simplified version of this, verbosity comes from checking T and T_Other are same type
    template<typename T_Other, class =
        std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T_Other>> && is_const::value && !std::is_const_v<T_Other>>>
    wrapped_amorphic_vec_iterator(const wrapped_amorphic_vec_iterator<T_Other>& other):
        m_base_it(other.m_base_it)
    {}

    template<typename T_Other, class =
        std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T_Other>> && is_const::value && !std::is_const_v<T_Other>>>
    wrapped_amorphic_vec_iterator& operator=(const wrapped_amorphic_vec_iterator<T_Other>& other)
    {
        m_base_it = other.m_base_it;
    }

    inline const internal::type_info& get_type_info() const {
        return m_base_it.get_type_info;
    }

    T& operator*() {
        return *reinterpret_cast<T*>(m_base_it.data());
    }

    const T& operator*() const {
        return *reinterpret_cast<const T*>(m_base_it.data());
    }

    T* operator->() {
        return reinterpret_cast<T*>(m_base_it.data());
    }

    const T* operator->() const {
        return reinterpret_cast<const T*>(m_base_it.data());
    }

    wrapped_amorphic_vec_iterator& operator++()
    {
        ++m_base_it;
        return *this;
    }

    wrapped_amorphic_vec_iterator& operator--()
    {
        --m_base_it;
        return *this;
    }

    wrapped_amorphic_vec_iterator operator++(int) {
        return wrapped_amorphic_vec_iterator(m_base_it++);
    }

    wrapped_amorphic_vec_iterator operator+(int n) const {
        return wrapped_amorphic_vec_iterator(m_base_it + n);
    }

    wrapped_amorphic_vec_iterator operator-(int n) const {
        return wrapped_amorphic_vec_iterator(m_base_it - n);
    }

    wrapped_amorphic_vec_iterator& operator+=(int n) {
        m_base_it += n;
        return *this;
    }

    wrapped_amorphic_vec_iterator& operator-=(int n) {
        m_base_it -= n;
        return *this;
    }

    template<typename T_Other>
    std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T_Other>>, bool>
    operator==(const wrapped_amorphic_vec_iterator<T_Other>& rhs) const {
        return m_base_it == rhs.m_base_it;
    }

    template<typename T_Other>
    std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T_Other>>, bool>
    operator!=(const wrapped_amorphic_vec_iterator<T_Other>& rhs) const {
        return m_base_it != rhs.m_base_it;
    }

    template<typename T_Other>
    std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T_Other>>, bool>
    operator<(const wrapped_amorphic_vec_iterator<T_Other>& rhs) const {
        return m_base_it < rhs.m_base_it;
    }

    template<typename T_Other>
    std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T_Other>>, bool>
    operator<=(const wrapped_amorphic_vec_iterator<T_Other>& rhs) const {
        return m_base_it <= rhs.m_base_it;
    }

    template<typename T_Other>
    std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T_Other>>, bool>
    operator>(const wrapped_amorphic_vec_iterator<T_Other>& rhs) const {
        return m_base_it > rhs.m_base_it;
    }

    template<typename T_Other>
    std::enable_if_t<std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T_Other>>, bool>
    operator>=(const wrapped_amorphic_vec_iterator<T_Other>& rhs) const {
        return m_base_it >= rhs.m_base_it;
    }

private:

    // should work?
    template<bool IsConst>
    wrapped_amorphic_vec_iterator(const amorphic_vec_iterator_t<IsConst>& it):
        m_base_it(it)
    {
        assert(internal::get_type_id<T>() == it.get_type_info().id);
    }

    base_it m_base_it;
};

template<typename T>
inline std::enable_if_t<!std::is_const_v<T>, void> wrapped_amorphic_iter_swap(const wrapped_amorphic_vec_iterator<T>& a, const wrapped_amorphic_vec_iterator<T>& b)
{
    unsigned char temp[sizeof(T)];

    memcpy(temp, &*a, sizeof(T));
    memcpy(&*a, &*b, sizeof(T));
    memcpy(&*b, temp, sizeof(T));
}

template<typename T>
class wrapped_amorphic_vec final
{
public:
    typedef wrapped_amorphic_vec_iterator<T> iterator;
    typedef wrapped_amorphic_vec_iterator<const T> const_iterator ;

    wrapped_amorphic_vec(amorphic_vec* wrapped_vec):
        m_wrapped_vec(wrapped_vec)
    {
        assert(m_wrapped_vec != nullptr);
        assert(m_wrapped_vec->get_type_info().id == internal::get_type_id<T>());
    }

    amorphic_vec* get_wrapped_vec() const {
        return m_wrapped_vec;
    }

    inline const internal::type_info& get_type_info() const {
        return m_wrapped_vec->get_type_info();
    }

    void pop_back() {
        m_wrapped_vec->pop_back();
    }

    void push_back(const T& value) {
        m_wrapped_vec->push_back(value);
    }

    template<typename... Args>
    T& emplace_back(Args&&... args) {
        return *new(m_wrapped_vec->push_back(nullptr)) T(std::forward<Args>(args)...);
    }


    T& operator[](size_t i) {
        return *reinterpret_cast<T*>((*m_wrapped_vec)[i]);
    }

    const T& operator[](size_t i) const {
        return *reinterpret_cast<const T*>((*m_wrapped_vec)[i]);
    }

    void reserve(const size_t capacity) {
        m_wrapped_vec->reserve(capacity);
    }

    void shrink_to_fit() {
        m_wrapped_vec->shrink_to_fit();
    }

    void clear() {
        m_wrapped_vec->clear();
    }

    size_t size() const {
        return m_wrapped_vec->size();
    }

    T* front() {
        return reinterpret_cast<T*>(m_wrapped_vec->front());
    }

    const T* front() const {
        return reinterpret_cast<const T*>(m_wrapped_vec->front());
    }

    T* back() {
        return reinterpret_cast<T*>(m_wrapped_vec->back());
    }

    const T* back() const {
        return reinterpret_cast<const T*>(m_wrapped_vec->back());
    }

    T* data() {
        return reinterpret_cast<T*>(m_wrapped_vec->data());
    }

    const T* data() const {
        return reinterpret_cast<const T*>(m_wrapped_vec->data());
    }

    iterator begin() {
        return iterator(m_wrapped_vec->begin());
    }

    const_iterator begin() const {
        return const_iterator(m_wrapped_vec->cbegin());
    }

    const_iterator cbegin() const {
        return const_iterator(m_wrapped_vec->cbegin());
    }

    iterator end() {
        return iterator(m_wrapped_vec->end());
    }

    const_iterator end() const {
        return const_iterator(m_wrapped_vec->cend());
    }

    const_iterator cend() const {
        return const_iterator(m_wrapped_vec->cend());
    }

private:

    // pointer or ref?
    amorphic_vec* m_wrapped_vec;
};

} /* namespace ecs::container */

#endif /* end of include guard: AMORPHIC_VECTOR_WRAPPER_H */
