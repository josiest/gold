#include "gold/button.h"

// frameworks
#include <SDL.h>

// data types
#include <cstdint>

using uint = std::uint32_t;

namespace au {

button::button(SDL_Rect const & bounds,
               SDL_Color const & border_color, SDL_Color const & fill_color,
               uint border_thickness, SDL_Texture * content, uint padding)

    : _bounds(bounds),
      _border_color(border_color), _fill_color(fill_color),
      _border_thickness(static_cast<int>(border_thickness)),
      _content(content), _padding(static_cast<int>(padding))
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

    // render the button content
    SDL_Rect const src = _clipped_texture_bounds();
    SDL_Rect const dst = _content_bounds();
    SDL_RenderCopy(renderer, _content, &src, &dst);
}

SDL_Rect button::_inner_bounds() const
{
    return {
        _bounds.x + _border_thickness, _bounds.y + _border_thickness,
        _bounds.w - 2*_border_thickness, _bounds.h - 2*_border_thickness
    };
}

SDL_Rect button::_max_content_bounds() const
{
    SDL_Rect const inner_bounds = _inner_bounds();
    return {
        inner_bounds.x + _padding, inner_bounds.y + _padding,
        inner_bounds.w - 2*_padding, inner_bounds.h - 2*_padding
    };
}

SDL_Rect button::_content_bounds() const
{
    SDL_Rect const max_bounds = _max_content_bounds();

    // normalize the texture height to the max bounds height
    int texture_width = 0;
    int texture_height = 0;
    SDL_QueryTexture(_content, nullptr, nullptr,
                               &texture_width, &texture_height);
    double const resize_ratio =
        max_bounds.h / static_cast<double>(texture_height);
    int const content_width = static_cast<int>(texture_width * resize_ratio);

    return {
        max_bounds.x, max_bounds.y,
        std::min(max_bounds.w, content_width), max_bounds.h
    };
}

SDL_Rect button::_clipped_texture_bounds() const
{
    SDL_Rect const max_bounds = _max_content_bounds();

    // normalize the texture height to the max bounds height
    int texture_width = 0;
    int texture_height = 0;
    SDL_QueryTexture(_content, nullptr, nullptr,
                               &texture_width, &texture_height);
    double const resize_ratio =
        max_bounds.h / static_cast<double>(texture_height);

    // clip the texture width if it's too long
    double const content_width = texture_width * resize_ratio;
    if (content_width > max_bounds.w) {
        texture_width = static_cast<int>(max_bounds.w/resize_ratio);
    }

    return { 0, 0, texture_width, texture_height };
}
}
