#include "gold/button_bag.h"

// frameworks and interfaces
#include <SDL.h>
#include <SDL_ttf.h>
#include "gold/widget.h" // iwidget_factory

// data structures and resource handlers
#include <vector>
#include <memory> // std::unique_ptr

// data types
#include "gold/button.h"
#include <cstdint> // std::uint32_t

using uint = std::uint32_t;

namespace au {

button_bag::button_bag(
        TTF_Font * font,
        std::uint32_t border_thickness, std::uint32_t padding,
        SDL_Color const & standard_color, SDL_Color const & hover_color,
        SDL_Color const & click_color, SDL_Color const & fill_color)

    : _font(font),
      _border_thickness(border_thickness), _padding(padding),
      _standard_color(standard_color), _hover_color(hover_color),
      _click_color(click_color), _fill_color(fill_color)
{
}

iwidget * button_bag::make(SDL_Renderer * renderer, std::string const & text,
                           SDL_Rect const & bounds)
{
    // render the text white to get pure color modulation capability
    static SDL_Color const white{0xff, 0xff, 0xff, 0xff};

    SDL_Texture * rendered_text = nullptr;
    // if the text has already been rendered, get its observer
    if (_texts.contains(text)) {
        rendered_text = _texts[text].get();
    }
    // otherwise render it, convert the surface to a texture
    // and add it to the resource manager
    else {
        SDL_Surface * text_surface =
            TTF_RenderText_Solid(_font, text.c_str(), white);

        rendered_text = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_FreeSurface(text_surface);

        _texts[text] = std::unique_ptr<SDL_Texture, texture_deleter>(
                rendered_text, texture_deleter{}
                );
    }

    return _buttons.emplace_back(new button(
                bounds, _border_thickness, _padding,
                _standard_color, _hover_color, _click_color, _fill_color,
                rendered_text
                )).get();
}
}
