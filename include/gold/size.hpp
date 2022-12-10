#pragma once
#include "gold/component.hpp"
#include "imgui/imgui.h"
#include <ranges>
#include <yaml-cpp/yaml.h>

inline namespace gold {
/** A widget will align_cursor with the desired size. */
struct size {
    float width = 200.f;
    float height = 40.f;

    [[nodiscard]] constexpr ImVec2 vector() const;
};
float sq_dist(gold::size const & lhs, gold::size const & rhs);
template<>
struct component_info<gold::size> {
    static constexpr std::string_view public_name = "Size";
};
}
constexpr ImVec2 gold::size::vector() const
{
    return ImVec2{ width, height };
}
namespace konbu{
template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config, gold::size & size, error_output & errors);
}
namespace YAML {
template<>
struct convert<gold::size> {
    static Node encode(gold::size const & size);
};
}
#include "gold/impl/size.tcc"