#ifndef TYPEINFO_H
#define TYPEINFO_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ecs::core {

template <typename X, typename T, typename... TN>
constexpr typename std::enable_if_t<std::is_same_v<X, T>, std::size_t> get_variadic_type_index(const std::size_t I = 0) {
    return I;
}

template <typename X, typename T, typename... TN>
constexpr typename std::enable_if_t<!std::is_same_v<X, T>, std::size_t> get_variadic_type_index(const std::size_t I = 0) {
    static_assert(sizeof...(TN) > 0, "Type not in variadic template list");
    return get_variadic_type_index<X, TN...>(I + 1);
}

template <typename X, typename... TN>
struct type_index {
    static const std::size_t index = get_variadic_type_index<X, TN...>();
};

} /* namespace ecs::core */

#endif /* end of include guard: TYPEINFO_H */
