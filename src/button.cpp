#include "gold/button.hpp"

// frameworks
#include <SDL.h>

// data types
#include <cstdint>

// math
#include "gold/geometry.hpp"

using uint = std::uint32_t;

namespace au {

button::button(SDL_Rect const & bounds, uint border_thickness, uint padding,

               SDL_Color const & standard_color, SDL_Color const & hover_color,
               SDL_Color const & click_color, SDL_Color const & fill_color,

               SDL_Texture * content)

    : _bounds(bounds),
      _border_thickness(static_cast<int>(border_thickness)),
      _padding(static_cast<int>(padding)),

      _standard_color(standard_color), _hover_color(hover_color),
      _click_color(click_color), _fill_color(fill_color),

      _content(content)
{
}

void button::render(SDL_Renderer * renderer)
{
    // set the respective border color and content texture
    // based on the mouse position
    SDL_Color draw_color = _standard_color;
    if (_mouse_in_bounds() and _mouse_clicked()) {
        draw_color = _click_color;
    }
    else if (_mouse_in_bounds()) {
        draw_color = _hover_color;
    }

    // fill from the outer border
    SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g,
                                     draw_color.b, draw_color.a);
    SDL_RenderFillRect(renderer, &_bounds);

    // fill from the inner border
    SDL_Rect const inner_bounds = _inner_bounds();
    SDL_SetRenderDrawColor(renderer, _fill_color.r, _fill_color.g,
                                     _fill_color.b, _fill_color.a);
    SDL_RenderFillRect(renderer, &inner_bounds);

    // render the button content
    SDL_Rect const src = clip_width(_texture_bounds(), _max_content_bounds());
    SDL_Rect const dst = _content_bounds();
    SDL_SetTextureColorMod(_content, draw_color.r, draw_color.g, draw_color.b);
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

    // scale the texture bounds into rendering space
    SDL_Rect const texture_bounds = _texture_bounds();
    double const rendering_scale = scale_by_height(texture_bounds, max_bounds);

    SDL_Rect content_bounds(max_bounds);
    content_bounds.w = static_cast<int>(texture_bounds.w * rendering_scale);

    // center the x position
    content_bounds.x = max_bounds.x + (max_bounds.w - content_bounds.w)/2;

    // clamp the content bounds to be within the maximum bounds
    content_bounds.x = std::max(max_bounds.x, content_bounds.x);
    content_bounds.w = std::min(max_bounds.w, content_bounds.w);

    return content_bounds;
}

SDL_Rect button::_texture_bounds() const
{
    SDL_Rect texture_bounds{0, 0, 0, 0};
    SDL_QueryTexture(_content, nullptr, nullptr,
                               &texture_bounds.w, &texture_bounds.h);
    return texture_bounds;
}

bool button::_mouse_in_bounds() const
{
    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    return x >= _bounds.x && x <= _bounds.x + _bounds.w
        && y >= _bounds.y && y <= _bounds.y + _bounds.h;
}

bool button::_mouse_clicked() const
{
    return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) != 0;
}
}
