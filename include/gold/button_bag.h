#pragma once

// frameworks and interfaces
#include <SDL.h>
#include <SDL_ttf.h>
#include "gold/widget.h" // iwidget_factory

// data structures and resource handlers
#include <vector>
#include <unordered_map>
#include <memory> // std::unique_ptr

// data types
#include "gold/button.h"
#include <cstdint> // std::uint32_t

namespace au {

struct texture_deleter {
    void operator()(SDL_Texture * texture) { SDL_DestroyTexture(texture); }
};

class button_bag : public iwidget_factory {
public:
    button_bag() = delete;
    button_bag(TTF_Font * font,
               std::uint32_t border_thickness, std::uint32_t padding,
               SDL_Color const & standard_color, SDL_Color const & hover_color,
               SDL_Color const & click_color, SDL_Color const & fill_color);

    iwidget * make(SDL_Renderer * renderer, std::string const & text,
                   SDL_Rect const & bounds);
private:
    std::vector<std::unique_ptr<button>> _buttons;
    std::unordered_map<std::string, std::unique_ptr<
            SDL_Texture, texture_deleter
            >> _texts;

    TTF_Font * _font;
    std::uint32_t _border_thickness;
    std::uint32_t _padding;

    SDL_Color _standard_color;
    SDL_Color _hover_color;
    SDL_Color _click_color;
    SDL_Color _fill_color;
};
}
