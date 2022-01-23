#pragma once

// frameworks
#include <SDL.h>
#include <SDL_ttf.h>

// data types
#include <string>

// data structures and resource handlers
#include <unordered_map>
#include <memory> // std::unique_ptr
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

// destroy various sdl resources
struct sdl_deleter {
    void operator()(SDL_Texture * texture) { SDL_DestroyTexture(texture); }
    void operator()(TTF_Font * font) { TTF_CloseFont(font); }
};

// aliases
using unique_texture = std::unique_ptr<SDL_Texture, sdl_deleter>;

using color_table = std::unordered_map<std::string, SDL_Color>;
using font_table = std::unordered_map<std::string, TTF_Font *>;

using unique_font = std::unique_ptr<TTF_Font, sdl_deleter>;
using unique_font_table = std::unordered_map<std::string, unique_font>;

/** Load color definitions from a yaml file */
result<color_table> load_colors(std::filesystem::path const & path);

/** Load fonts from a directory of fonts */
result<unique_font_table>
load_all_fonts(std::filesystem::path const & dir, std::uint32_t resolution = 100);

/** Get a font table of observing pointers. */
font_table observe_fonts(unique_font_table const & fonts);
}
