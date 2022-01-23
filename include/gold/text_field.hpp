#pragma once

// frameworks
#include <SDL.h>

// data types
#include "gold/widget.hpp"
#include <cstdint>

// resource handlers
#include "gold/serialization.hpp"

namespace au {

class text_field : public itext_widget, public imonochrome {
public:
    text_field() = delete;
    text_field(SDL_Renderer * renderer, SDL_Rect const & bounds,
               TTF_Font * font, SDL_Color const & text_color,
               std::string const & text);

    // interface methods

    // text_widget
    inline SDL_Rect bounds() const { return _bounds; }
    void render(SDL_Renderer * renderer);

    inline std::string get_text() const { return _text; }
    void set_text(std::string const & text);

    // monochrome
    inline SDL_Color get_color() const { return _text_color; }
    void set_color(SDL_Color const & color) { _text_color = color; }
private:
    SDL_Renderer * _renderer;
    SDL_Rect _bounds;
    TTF_Font * _font;
    SDL_Color _text_color;
    std::string _text;
    unique_texture _content;

    SDL_Texture * _render_text(std::string const & text);
    SDL_Rect _texture_bounds() const;
    SDL_Rect _content_bounds() const;
};
}
