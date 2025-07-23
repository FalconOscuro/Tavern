#ifndef COMPONENT_TYPE_H
#define COMPONENT_TYPE_H
// should be in core

#include <cstddef>
#include <cstring>
#include <cstdint>
#include <limits>
#include <string_view>
#include <utility>

#include "../config/config.h"
#include "../containers/component_container.hpp"

namespace ecs::core {

namespace internal {

template <size_t SIZE>
class string_literal
{
    static_assert(SIZE > 0, "string_literal size must be non-zero");
public:

    constexpr string_literal(const char (&c_str)[SIZE]):
        string_literal(static_cast<const char*>(c_str))
    {}

    constexpr string_literal(const char* c_str) {
        for (size_t i = 0; i < SIZE; ++i)
            m_data[i] = c_str[i];
    }

    template <size_t L_SIZE, size_t R_SIZE>
    constexpr string_literal(const string_literal<L_SIZE>& lhs, const string_literal<R_SIZE>& rhs) {
        static_assert(L_SIZE + R_SIZE - 1 == SIZE, "Assigned string_literal size is not big enough to concatenate arguments!");

        for (size_t i = 0; i < L_SIZE - 1; ++i)
            m_data[i] = lhs.m_data[i];

        for (size_t i = 0; i < R_SIZE; ++i)
            m_data[L_SIZE - 1 + i] = rhs.m_data[i];
    }

    constexpr size_t find_first_of(char c) const {
        for (size_t i = 0; i < SIZE; ++i)
             if (m_data[i] == c)
                return i;

        return SIZE;
    }

    template <size_t POS, size_t COUNT>
    constexpr string_literal<COUNT> substr() const {
        static_assert(POS < SIZE, "substr start pos out of range");
        static_assert(POS + COUNT <= SIZE, "substr count out of range");

        return string_literal<COUNT>(static_cast<const char*>(m_data) + POS);
    }

    constexpr operator std::string_view() const {
        return std::string_view{static_cast<const char*>(m_data), SIZE - 1};
    };

private:
    char m_data[SIZE] = {};
}; /* end of class string_literal */

// Get unmangled type name from pretty function macro at compile time
template<typename type>
[[nodiscard]] constexpr std::enable_if_t<!std::is_const_v<type> && !std::is_volatile_v<type>, const std::string_view> get_type_name() noexcept
{
    //constexpr auto func_sig = string_literal(__FUNC_PRETTY__);

    //constexpr size_t start = func_sig.find_first_of(FUNC_PRETTY_PREFIX) + 2;
    //constexpr size_t end   = func_sig.find_first_of(FUNC_PRETTY_END);
    //constexpr size_t len   = end - start;

    //return func_sig.template substr<start, len>();

    std::string_view func_sig{static_cast<const char*>(__FUNC_PRETTY__)};

    auto start = func_sig.find_first_of(FUNC_PRETTY_PREFIX) + 2;
    auto end = func_sig.find_first_of(FUNC_PRETTY_END);
    auto name = func_sig.substr(start, end - start);

    return name;

    //char* c_str = new char[name.length() + 1];

    //// using strcpy can cause compiler warnings
    //memcpy(c_str, name.data(), name.length());
    //c_str[name.length()] = '\0';

    //return c_str;
}

// unsupported pointer and reference!
template<typename type>
[[nodiscard]] constexpr std::enable_if_t<std::is_const_v<type> || std::is_volatile_v<type>, const std::string_view> get_type_name() noexcept {
    return get_type_name<std::remove_cv_t<type>>();
}

template<typename type>
[[nodiscard]] constexpr size_t get_type_id() noexcept {
    return std::hash<std::string_view>{}(get_type_name<std::remove_cv_t<type>>());
}

typedef void (*construct_type)(void*);
typedef void (*destroy_type)(void*);

template<typename T>
struct templated_type_funcs
{
    static void constructor(void* ptr) {
        new(ptr) std::remove_cv_t<T>;
    }

    static void destructor(void* ptr) {
        reinterpret_cast<T*>(ptr)->~T();
    }

}; /* end of struct templated_type_funcs */

} /* end of namespace internal */

/*! \brief Information for singular type
 *
 *  Contains unmanged type name and unique id
 */
struct type_info final
{
    template<typename component_type>
    constexpr type_info(std::in_place_type_t<container::component_container<component_type>>):
        id(internal::get_type_id<container::component_container<component_type>>()),
        name(internal::get_type_name<container::component_container<component_type>>()),
        size(sizeof(container::component_container<component_type>)),
        constructor(internal::templated_type_funcs<container::component_container<component_type>>::constructor),
        destructor(internal::templated_type_funcs<container::component_container<component_type>>::destructor),
        align(alignof(container::component_container<component_type>)),
        is_valid_component_container(true)
    {}

    // Compile time definition using templating
    template<typename type>
    constexpr type_info(std::in_place_type_t<type>):
        id(internal::get_type_id<type>()),
        name(internal::get_type_name<type>()),
        size(sizeof(type)),
        constructor(internal::templated_type_funcs<type>::constructor),
        destructor(internal::templated_type_funcs<type>::destructor),
        align(alignof(type)),
        is_valid_component_container(false)
    {}

    // runtime definition using unmangled type name
    // default alignment is max alignment
    type_info(
        std::string_view type_name,
        internal::construct_type constructor,
        internal::destroy_type destructor,
        const uint64_t size,
        bool is_valid_component_container = false,
        const uint8_t align = alignof(std::max_align_t)
    ):
        id(std::hash<std::string_view>{}(type_name)),
        name(type_name),
        size(size),
        constructor(constructor),
        destructor(destructor),
        align(align),
        is_valid_component_container(is_valid_component_container)
    {}

    ~type_info() = default;

    type_info(const type_info&) = default;
    type_info(type_info&&) = default;

    const uint64_t id;
    const std::string_view name;

    const uint64_t size;

    const internal::construct_type constructor;
    const internal::destroy_type destructor;

    // align unlikely to be more than 16 bytes as max align_t
    const uint8_t align;
    static_assert(alignof(std::max_align_t) <= std::numeric_limits<decltype(align)>::max(), "Max align greater than storable value!");

    const bool is_valid_component_container;

    // un-used module_id info, to prevent cross module collisions
    //const uint32_t module_id;
    //const std::string_view module_name;

    // comparison assumes correct construction
    bool operator==(const type_info& rhs) const
    {
        // Fast check of ID, then need to check for hash collisions
        // additional fast check of name memory address, can assume if pointing to same string, is of same type
        return id == rhs.id && (
            name.data() == rhs.name.data()
            || name == rhs.name
        );
    }

    bool operator!=(const type_info& rhs) const
    {
        // Account for hashing collisions,
        return id != rhs.id || (
            name.data() != rhs.name.data()
            && name != rhs.name
        );
    }
}; /* struct type_info */

} /* namespace ecs::core */

namespace std {

template<>
class hash<ecs::core::type_info>
{
public:
    // NOTE: not final, eventually hash function to use identity of combined id's for type and source
    // both 32bit will form single 64bit unique int
    uint64_t operator()(const ecs::core::type_info& type_info) const {
        return hash<string_view>{}(type_info.name);
    }
}; /* class hash<ecs::core::type_info> */

} /* namespace std */

#endif /* end of include guard: COMPONENT_TYPE_H */
