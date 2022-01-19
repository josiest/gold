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
           std::uint32_t border_thickness, std::uint32_t padding,

           SDL_Color const & standard_color, SDL_Color const & hover_color,
           SDL_Color const & click_color, SDL_Color const & fill_color,

           SDL_Texture * content);

    // interface methods
    inline SDL_Rect bounds() const { return _bounds; }
    void render(SDL_Renderer * renderer);
private:
    SDL_Rect _bounds;
    int _border_thickness;
    int _padding;

    SDL_Color _standard_color;
    SDL_Color _hover_color;
    SDL_Color _click_color;
    SDL_Color _fill_color;

    SDL_Texture * _content;

    // get the bounding rect of the button inside the border
    SDL_Rect _inner_bounds() const;

    // get the maximum bounding rect for the inner content
    SDL_Rect _max_content_bounds() const;

    // get the bounding rect to draw the texture to
    SDL_Rect _content_bounds() const;

    // get the bounding rect to draw the texture from
    SDL_Rect _clipped_texture_bounds() const;

    // determine if the mouse is in the bounds of this button
    bool _mouse_in_bounds() const;

    // determine of the mouse was clicked
    bool _mouse_clicked() const;
};
}
