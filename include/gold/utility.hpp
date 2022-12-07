#pragma once
#include <concepts>
#include <iostream>

namespace gold {

namespace align {
enum class horizontal;
enum class vertical;
}

std::string to_string(align::horizontal const & horizontal);

template<typename value>
concept string_convertible =
requires(value const & v) {
    { gold::to_string(v) } -> std::convertible_to<std::string>;
};
}
namespace std {
template<gold::string_convertible value>
std::ostream & operator<<(std::ostream & os, value const & v) {
    return os << gold::to_string(v);
}
}