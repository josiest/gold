#pragma once
#include <functional>
#include <concepts>
#include <cstdint>
#include <unordered_map>

namespace ion {

/**
 * \brief A function that hashes a type
 * \tparam F    the hash function for the type
 * \tparam T    the type to hash
 */
template<typename F, typename T>
concept hash_for = std::regular_invocable<F, T> and
requires(F && hash_fn, T && elem)
{
    { std::invoke(std::forward<F>(hash_fn),
                  std::forward<T>(elem)) }
        -> std::convertible_to<std::size_t>;
};

/**
 * \brief a basic unordered histogram type
 * \tparam T    the type to count
 */
template<std::regular T, hash_for<T> Hash = std::hash<T>>
using histogram = std::unordered_map<T, std::uint32_t, Hash>;

/**
 * \brief Add a histogram to another histogram
 * \tparam T            the type to count
 *
 * \param cumulative    the histogram to add to
 * \param transient     the histogram to add from
 */
template<std::regular T, hash_for<T> Hash>
auto add_all(histogram<T, Hash> & cumulative,
             histogram<T, Hash> const & transient)
{
    for (auto const &[key, count] : transient) {
        cumulative[key] += count;
    }
}
}
