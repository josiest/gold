#pragma once

// frameworks and interfaces
#include <SDL.h>
#include "gold/widget.hpp"
#include <tl/expected.hpp>

// data types
#include <cstdint>
#include <string>
#include <filesystem>

namespace au {

class frame {
public:
    frame(SDL_Renderer * renderer, SDL_Rect const & bounds,
          std::uint32_t button_height, std::uint32_t padding);

    frame(SDL_Renderer * renderer, int x, int y, std::uint32_t w, std::uint32_t h,
          std::uint32_t button_height, std::uint32_t padding);

    result<iwidget *> produce_text_widget(widget_factory auto & factory,
                                          std::string const & text)
    {
        SDL_Rect const bounds{
            _next.x, _next.y, _bounds.w - 2*_padding, _button_height
        };
        _next.y += _button_height + _padding;

        auto widget = factory.make_text_widget(_renderer, text, bounds);
        if (widget) {
            _widgets.push_back(*widget);
        }
        return widget;
    }

    void render();

    static result<frame> from_file(SDL_Renderer * renderer,
                                   SDL_Rect const & bounds,
                                   std::filesystem::path const & path);
private:
    SDL_Renderer * _renderer;
    SDL_Rect _bounds;
    int _button_height;
    int _padding;
    SDL_Point _next;
    std::vector<iwidget *> _widgets;
};
}
