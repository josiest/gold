#pragma once

// frameworks and interfaces
#include <SDL.h>
#include "gold/widget.hpp"

// data types
#include <cstdint>
#include <string>

namespace au {

class frame {
public:
    frame(SDL_Renderer * renderer, SDL_Rect const & bounds,
          std::uint32_t button_height, std::uint32_t padding);

    frame(SDL_Renderer * renderer, int x, int y, std::uint32_t w, std::uint32_t h,
          std::uint32_t button_height, std::uint32_t padding);

    result<iwidget *> produce_widget(widget_factory auto & factory,
                                     std::string const & text)
    {
        SDL_Rect const bounds{
            _next.x, _next.y, _bounds.w - 2*_padding, _button_height
        };
        _next.y += _button_height + _padding;

        return factory.make_widget(_renderer, text, bounds);
    }
private:
    SDL_Renderer * _renderer;
    SDL_Rect _bounds;
    int _button_height;
    int _padding;
    SDL_Point _next;
};
}
