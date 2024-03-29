#pragma once
#include "gold/component.hpp"
#include <string>
#include <ranges>
#include <yaml-cpp/yaml.h>

inline namespace gold {
namespace align {
/** Horizontal alignment setting */
enum class horizontal {
    left,   /** Widget should be left-aligned */
    right,  /** Widget should be right-aligned */
    center, /** Widget should be centered horizontally */
    fill    /** Widget should horizontally fill its layout */
};
/** Vertical alignment setting */
enum class vertical {
    top,    /** Widget should be anchored to the top */
    bottom, /** Widget should be anchored to the bottom */
    center, /** Widget should be centered vertically */
    fill    /** Widget should vertically fill its layout */
};
}

/** Define how a widget will be aligned in the layout */
struct layout {
    align::horizontal horizontal = align::horizontal::left;
    align::vertical vertical = align::vertical::top;

    bool constexpr operator==(layout const & rhs) const = default;
};

std::string to_string(align::horizontal const & horizontal);
std::string to_string(align::vertical const & vert);

template<>
struct component_info<gold::layout> {
    using type = gold::layout;
    static constexpr std::string_view public_name = "Alignment";
};
}

namespace konbu {

template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          gold::align::horizontal & halign,
          error_output & errors);

template <std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          gold::align::vertical & valign,
          error_output & errors);

template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          gold::layout & layout,
          error_output & errors);
}

namespace YAML {
template<>
struct convert<gold::align::horizontal> {
    static Node encode(gold::align::horizontal halign);
};

template<>
struct convert<gold::align::vertical> {
    static Node encode(gold::align::vertical valign);
};
template<>
struct convert<gold::layout> {
    static Node encode(gold::layout layout);
};
Emitter & operator<<(Emitter & out, gold::layout layout);
}
#include "gold/impl/layout.tcc"