#pragma once

#include <entt/entity/registry.hpp>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace gold {
YAML::Emitter &
write(YAML::Emitter & out, entt::registry const & widgets,
                           entt::entity widget);

bool write(std::filesystem::path const & path,
           entt::registry const & widgets,
           entt::entity widget);
}