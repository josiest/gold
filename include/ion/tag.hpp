#pragma once

// serializeation
#include <yaml-cpp/yaml.h>
#include "konbu/konbu.h"
#include <iostream>

// data types
#include <string>

namespace ion {

/**
 * \brief A string-type that supports dot-separated tagging operations
 */
class tag {
public:
    inline tag() = default;

    inline explicit tag(std::string str) : _str{ std::move(str) } {}
    inline explicit tag(char const * str) : _str{ str } {}
    [[nodiscard]] inline std::string string() const { return _str; }
    [[nodiscard]] inline char const * c_str() const { return _str.c_str(); }
    auto operator<=>(tag const & rhs) const = default;
private:
    std::string _str;
};
}
std::ostream & operator<<(std::ostream & os, ion::tag const & tag);
std::istream & operator>>(std::istream & is, ion::tag & tag);

namespace std {
template<>
struct hash<ion::tag> {
    size_t operator()(ion::tag const& tag) const;
};
}
namespace konbu {
template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config, ion::tag & value, error_output & errors);
}
namespace YAML {
template<>
struct convert<ion::tag> {
    static Node encode(ion::tag const & tag);
};
}

inline std::size_t
std::hash<ion::tag>::operator()(ion::tag const & tag) const
{
    std::hash<string> str_hash;
    return str_hash(tag.string());
}
template<std::ranges::output_range<YAML::Exception> error_output>
inline void konbu::read(YAML::Node const & config,
                        ion::tag & value,
                        error_output & errors)
{
    std::string input;
    konbu::read(config, input, errors);
    value = ion::tag{ input };
}
inline YAML::Node
YAML::convert<ion::tag>::encode(ion::tag const & tag)
{
    return Node{ tag.string() };
}