#include "gold/widget.hpp"
#include "gold/layout.hpp"
#include "gold/background_color.hpp"
#include "gold/size.hpp"

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
    if (auto const * layout = widgets.try_get<gold::layout>(widget)) {
        out << YAML::Key << "align"
            << YAML::Value << YAML::Flow << YAML::Node{ *layout };
    }
    if (auto const * size = widgets.try_get<gold::size>(widget)) {
        out << YAML::Key << "size"
            << YAML::Value << YAML::Flow << YAML::Node{ *size };
    }
    if (auto const * color = widgets.try_get<gold::background_color>(widget)) {
        out << YAML::Key << "bg-color"
            << YAML::Value << YAML::Flow << YAML::Node{ *color };
    }
    return out << YAML::EndMap;
}

bool gold::write(fs::path const & path, entt::registry const & widgets,
                                        entt::entity widget)
{
    YAML::Emitter out;
    write(out, widgets, widget);
    std::fstream file{ path };
    file << out.c_str();
    file.close();

    return true;
}