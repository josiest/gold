#pragma once
#include "imgui/imgui.h"
#include <ranges>
#include <yaml-cpp/yaml.h>

namespace gold {
/** A widget will align_cursor with the desired size. */
struct size {
    float width = 0.f;
    float height = 0.f;

    [[nodiscard]] constexpr ImVec2 vector() const;
};
float sq_dist(gold::size const & lhs, gold::size const & rhs);
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
#include "gold/size.tcc"