#include "gold/button.h"

// frameworks
#include <SDL.h>

// data types
#include <cstdint>

using uint = std::uint32_t;

namespace au {

button::button(SDL_Rect const & bounds, SDL_Color const & border_color,
               SDL_Color const & fill_color, uint border_thickness)

    : _bounds(bounds), _border_color(border_color), _fill_color(fill_color),
      _border_thickness(static_cast<int>(border_thickness))
{
}

void button::render(SDL_Renderer * renderer)
{
    // fill from the outer border
    SDL_SetRenderDrawColor(renderer, _border_color.r, _border_color.g,
                                     _border_color.b, _border_color.a);
    SDL_RenderFillRect(renderer, &_bounds);

    // fill from the inner border
    SDL_Rect const inner_bounds = _inner_bounds();
    SDL_SetRenderDrawColor(renderer, _fill_color.r, _fill_color.g,
                                     _fill_color.b, _fill_color.a);
    SDL_RenderFillRect(renderer, &inner_bounds);
}

SDL_Rect button::_inner_bounds() const
{
    return {
        _bounds.x + _border_thickness, _bounds.y + _border_thickness,
        _bounds.w - 2*_border_thickness, _bounds.h - 2*_border_thickness
    };
}
}
