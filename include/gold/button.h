#pragma once

// frameworks
#include <SDL.h>

// data types
#include "gold/widget.h"
#include <cstdint>

namespace au {

class button : public iwidget {
public:
    button() = delete;
    button(SDL_Rect const & bounds,

           SDL_Color const & border_color, SDL_Color const & fill_color,
           std::uint32_t border_thickness,

           SDL_Texture * content,
           std::uint32_t horizontal_padding, std::uint32_t vertical_padding);

    // interface methods
    inline SDL_Rect bounds() const { return _bounds; }
    void render(SDL_Renderer * renderer);
private:
    SDL_Rect _bounds;
    SDL_Color _border_color;
    SDL_Color _fill_color;
    int _border_thickness;

    SDL_Texture * _content;
    int _horizontal_padding;
    int _vertical_padding;

    // get the bounding rect of the button inside the border
    SDL_Rect _inner_bounds() const;

    // get the maximum bounding rect for the inner content
    SDL_Rect _max_content_bounds() const;

    // get the bounding rect to draw the texture to
    SDL_Rect _content_bounds() const;

    // get the bounding rect to draw the texture from
    SDL_Rect _clipped_texture_bounds() const;
};
}
