#include "gold/button.h"

// frameworks
#include <SDL.h>

// data types
#include <cstdint>

using uint = std::uint32_t;

namespace au {

button::button(SDL_Rect const & bounds, uint border_thickness, uint padding,

               SDL_Color const & border_color, SDL_Color const & hover_color,
               SDL_Color const & fill_color,

               SDL_Texture * content, SDL_Texture * hover_content)

    : _bounds(bounds),
      _border_thickness(static_cast<int>(border_thickness)),
      _padding(static_cast<int>(padding)),

      _border_color(border_color),
      _fill_color(fill_color),

      _content(content), _hover_content(hover_content)
{
}

void button::render(SDL_Renderer * renderer)
{
    // set the respective border color and content texture
    // based on the mouse position
    SDL_Color border_color = _border_color;
    SDL_Texture * content = _content;

    if (_mouse_in_bounds()) {
        border_color = _hover_color;
        content = _hover_content;
    }

    // fill from the outer border
    SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g,
                                     border_color.b, border_color.a);
    SDL_RenderFillRect(renderer, &_bounds);

    // fill from the inner border
    SDL_Rect const inner_bounds = _inner_bounds();
    SDL_SetRenderDrawColor(renderer, _fill_color.r, _fill_color.g,
                                     _fill_color.b, _fill_color.a);
    SDL_RenderFillRect(renderer, &inner_bounds);

    // render the button content
    SDL_Rect const src = _clipped_texture_bounds();
    SDL_Rect const dst = _content_bounds();
    SDL_RenderCopy(renderer, content, &src, &dst);
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

bool button::_mouse_in_bounds() const
{
    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    return x >= _bounds.x && x <= _bounds.x + _bounds.w
        && y >= _bounds.y && y <= _bounds.y + _bounds.h;
}
}
