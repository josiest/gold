#include "gold/widget.hpp"
#include "gold/layout.hpp"

#include <fstream>
#include <filesystem>
#include <entt/entity/registry.hpp>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;
YAML::Emitter &
gold::write(YAML::Emitter & out, entt::registry const & widgets,
                                 entt::entity widget)
{
    out << YAML::Block << YAML::BeginMap;
    if (auto * layout = widgets.try_get<gold::layout>(widget)) {
        gold::layout default_layout;
        if (default_layout != *layout) {
            out << YAML::Key << "layout"
                << YAML::Value << YAML::Flow << YAML::Node{ *layout };
        }
    }
    return out << YAML::EndMap;
}

bool gold::write(fs::path const & path, entt::registry const & widgets,
                                        entt::entity widget)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    if (auto * layout = widgets.try_get<gold::layout>(widget)) {
        gold::layout default_layout;
        if (default_layout != *layout) {
            out << YAML::Key << "layout"
                << YAML::Value << YAML::Flow << YAML::Node{ *layout };
        }
    }
    out << YAML::EndMap;
    std::fstream file{path};
    file << out.c_str();
    file.close();

    return true;
}