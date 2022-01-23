#pragma once

// frameworks
#include <SDL.h>

// data types
#include <string>

// data structures and resource handlers
#include <unordered_map>
#include "gold/result.hpp"

// i/o and serialization
#include <yaml-cpp/yaml.h>
#include <filesystem>

namespace YAML {
template<>
struct convert<SDL_Color> {
    static Node encode(SDL_Color const & color) {
        Node node;
        node.push_back(color.r);
        node.push_back(color.g);
        node.push_back(color.b);
        return node;
    }

    static bool decode(Node const & node, SDL_Color & color) {
        if(not node.IsSequence() or node.size() != 3) {
            return false;
        }
        for (auto const & value : node) {
            if (not value.IsScalar()) return false;
        }
        color.r = node[0].as<int>(0);
        color.g = node[1].as<int>(0);
        color.b = node[2].as<int>(0);
        return true;
    }
};
}

namespace au {
using color_table = std::unordered_map<std::string, SDL_Color>;

/** Load color definitions from a yaml file */
result<color_table> load_colors(std::filesystem::path const & path);

}
