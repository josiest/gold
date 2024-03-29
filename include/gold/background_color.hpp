#pragma once
#include "gold/component.hpp"
#include "imgui/imgui.h"
#include <ranges>
#include <yaml-cpp/yaml.h>

inline namespace gold {

/** A widget will align_cursor with the desired color. */
struct background_color {
    float red = .16f;
    float green = .65f;
    float blue = 1.f;
    float alpha = .5f;

    [[nodiscard]] constexpr ImVec4 vector() const;
};
constexpr float sq_dist(gold::background_color const & lhs,
                        gold::background_color const & rhs);

template<>
struct component_info<gold::background_color> {
    static constexpr std::string_view public_name = "Background Color";
};
}
namespace konbu {
template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config, gold::background_color & color,
                                     error_output & errors);
}
namespace YAML {
template<>
struct convert<gold::background_color> {
    static Node encode(gold::background_color const & color);
};
}
#include "gold/impl/background_color.tcc"