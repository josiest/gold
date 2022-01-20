#pragma once

// frameworks and interfaces
#include <SDL.h>
#include <SDL_ttf.h>
#include <yaml-cpp/yaml.h>
#include "gold/widget.hpp" // iwidget

// data structures and resource handlers
#include <vector>
#include <unordered_map>
#include <memory> // std::unique_ptr
#include <tl/expected.hpp>

// data types
#include "gold/button.hpp"
#include <cstdint>      // std::uint32_t
#include <filesystem>   // fs::path

namespace au {

using color_table = std::unordered_map<std::string, SDL_Color>;

/** A resource manager/factory class for gold buttons. */
class button_factory {
public:
    button_factory() = delete;

    /** Create a button factory that makes buttons with the specified settings. */
    button_factory(TTF_Font * font,
                   std::uint32_t border_thickness, std::uint32_t padding,
                   SDL_Color const & standard_color, SDL_Color const & hover_color,
                   SDL_Color const & click_color, SDL_Color const & fill_color);

    /** Make a new widget */
    result<iwidget *>
        make_widget(SDL_Renderer * renderer, std::string const & text,
                    SDL_Rect const & bounds);

    /** Load color definitions from a yaml file */
    static result<bool> load_colors(std::filesystem::path const & path);
private:
    static color_table _colors;

    std::vector<std::unique_ptr<button>> _buttons;
    std::unordered_map<std::string, unique_texture> _texts;

    TTF_Font * _font;
    std::uint32_t _border_thickness;
    std::uint32_t _padding;

    SDL_Color _standard_color;
    SDL_Color _hover_color;
    SDL_Color _click_color;
    SDL_Color _fill_color;
};
}

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
