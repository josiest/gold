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
using font_table = std::unordered_map<std::string, TTF_Font *>;

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
        make_text_widget(SDL_Renderer * renderer, std::string const & text,
                         SDL_Rect const & bounds);

    /** Load color definitions from a yaml file */
    static result<bool> load_colors(std::filesystem::path const & path);

    /** Load fonts from a directory of fonts */
    static result<std::vector<unique_font>>
        load_all_fonts(std::filesystem::path const & dir);

    /** Create a button factory from a button config file. */
    static result<button_factory> from_file(std::filesystem::path const & path);
private:
    // static color and font definitions for reference
    // when loading button configurations
    static color_table _colors;
    static font_table _fonts;

    // the factory owns all button and texture resources it creates
    std::vector<std::unique_ptr<button>> _buttons;
    std::unordered_map<std::string, unique_texture> _texts;

    // button configuration
    TTF_Font * _font;
    std::uint32_t _border_thickness;
    std::uint32_t _padding;

    SDL_Color _standard_color;
    SDL_Color _hover_color;
    SDL_Color _click_color;
    SDL_Color _fill_color;
};
}

