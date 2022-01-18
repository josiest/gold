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
    button(SDL_Rect const & bounds, SDL_Color const & border_color,
           SDL_Color const & fill_color, std::uint32_t border_thickness);

    // interface methods
    inline SDL_Rect bounds() const { return _bounds; }
    void render(SDL_Renderer * renderer);
private:
    SDL_Rect _bounds;
    SDL_Color _border_color;
    SDL_Color _fill_color;
    int _border_thickness;

    // get the bounding rect of the button inside the border
    SDL_Rect _inner_bounds() const;
};
}
