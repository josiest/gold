#pragma once

#include <entt/entity/registry.hpp>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <ranges>

namespace konbu {
template<std::ranges::output_range<YAML::Exception> error_output>
entt::entity read_widget(YAML::Node const & config,
                         entt::registry & widgets,
                         error_output & errors);
}
namespace gold {
YAML::Emitter &
write(YAML::Emitter & out, entt::registry const & widgets,
                           entt::entity widget);

bool write(std::filesystem::path const & path,
           entt::registry const & widgets,
           entt::entity widget);
}
#include "gold/impl/widget.tcc"