#pragma once

// frameworks and interfaces
#include <SDL.h>
#include <SDL_ttf.h>
#include "gold/widget.hpp" // iwidget
#include <tl/expected.hpp>

// data structures and resource handlers
#include <vector>
#include <unordered_map>
#include <memory> // std::unique_ptr

// data types
#include "gold/button.hpp"
#include <cstdint> // std::uint32_t

namespace au {

/** A resource manager/factory class for gold buttons. */
class button_factory {
public:
    button_factory() = delete;

    /** Create a button factory that makes buttons with the specified settings. */
    button_factory(TTF_Font * font,
                   std::uint32_t border_thickness, std::uint32_t padding,
                   SDL_Color const & standard_color, SDL_Color const & hover_color,
                   SDL_Color const & click_color, SDL_Color const & fill_color);

    /** Make a new widget */
    result<iwidget *>
        make_widget(SDL_Renderer * renderer, std::string const & text,
                    SDL_Rect const & bounds);
private:
    std::vector<std::unique_ptr<button>> _buttons;
    std::unordered_map<std::string, unique_texture> _texts;

    TTF_Font * _font;
    std::uint32_t _border_thickness;
    std::uint32_t _padding;

    SDL_Color _standard_color;
    SDL_Color _hover_color;
    SDL_Color _click_color;
    SDL_Color _fill_color;
};
}
