#pragma once

// frameworks and interfaces
#include <SDL.h>
#include <SDL_ttf.h>
#include "gold/widget.hpp"

// data types
#include "gold/text_field.hpp"

// data structures and resource handlers
#include <vector>
#include <memory>
#include "gold/serialization.hpp"

// i/o
#include <filesystem>

namespace au {

class text_factory {
public:
    /** Create a factory that makes text fields with the specified settings. */
    text_factory(TTF_Font * font, SDL_Color const & color);

    /** Make a new text field. */
    result<itext_widget *>
    make_text_widget(SDL_Renderer * renderer, std::string const & text,
                     SDL_Rect const & bounds);

    /** Add color definitions to the button factory. */
    static void update_colors(color_table const & table);

    /** Add font definitions to the button factory. */
    static void update_fonts(font_table const & table);

    /** Create a factory from a text config file. */
    static result<text_factory> from_file(std::filesystem::path const & path);

private:
    // static color and font definitions for reference
    // when loading text configurations
    static color_table _colors;
    static font_table _fonts;

    // the factory owns all text field resources it creates
    std::vector<std::unique_ptr<text_field>> _texts;

    // text configuration
    TTF_Font * _font;
    SDL_Color _color;
};
}
