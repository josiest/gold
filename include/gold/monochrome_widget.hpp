#pragma once

// frameworks
#include <SDL.h>

// data types
#include "gold/widget.hpp"
#include <cstdint>

namespace au {

class monochrome_widget : public iwidget {
public:
    monochrome_widget() = delete;
    monochrome_widget(SDL_Rect const & bounds, SDL_Color const & color_mod,
                      SDL_Texture * content);

    // interface methods
    inline SDL_Rect bounds() const { return _bounds; }
    void render(SDL_Renderer * renderer);
private:
    SDL_Rect _bounds;
    SDL_Color _color_mod;
    SDL_Texture * _content;

    SDL_Rect _texture_bounds() const;
    SDL_Rect _content_bounds() const;
};
}
