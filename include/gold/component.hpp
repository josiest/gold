#pragma once
#include <string_view>
#include <concepts>
#include <entt/entity/registry.hpp>

inline namespace gold {
template<typename component>
struct component_info {
};

template<typename component>
constexpr bool has_public_name = requires() {
    { component_info<component>::public_name }
        -> std::convertible_to<std::string_view>;
};

template<typename component>
concept editor_option = requires(component & v) {
    show_options(v);
};

template<gold::editor_option component_type>
requires gold::has_public_name<component_type>
void show_component_options(entt::registry & widgets, entt::entity widget);
}
#include "gold/impl/component.tcc"