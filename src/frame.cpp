#include "gold/frame.hpp"

// frameworks
#include <SDL.h>

// data types
#include <cstdint>

// aliases
using uint = std::uint32_t;

namespace au {

frame::frame(SDL_Renderer * renderer, SDL_Rect const & bounds,
             uint button_height, uint padding)
    : _renderer(renderer), _bounds(bounds),
      _button_height(static_cast<int>(button_height)),
      _padding(static_cast<int>(padding)),
      _next{bounds.x + _padding, bounds.y + _padding}
{
}
frame::frame(SDL_Renderer * renderer, int x, int y, uint w, uint h,
             uint button_height, uint padding)
    : frame(renderer, SDL_Rect{x, y, static_cast<int>(w), static_cast<int>(h)},
            button_height, padding)
{
}
}
