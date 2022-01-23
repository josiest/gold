#include "gold/monochrome_widget.hpp"

// frameworks
#include <SDL.h>

// math
#include "gold/geometry.hpp"

namespace au {

monochrome_widget::monochrome_widget(SDL_Rect const & bounds,
                             SDL_Color const & color_mod,
                             SDL_Texture * content)

    : _bounds(bounds), _color_mod(color_mod), _content(content)
{
}

void monochrome_widget::render(SDL_Renderer * renderer)
{
    // compute the src and dest rects
    SDL_Rect const src = clip_width(_texture_bounds(), _bounds);
    SDL_Rect const dst = _content_bounds();

    // change the color mod and render
    SDL_SetTextureColorMod(_content, _color_mod.r, _color_mod.g, _color_mod.b);
    SDL_RenderCopy(renderer, _content, &src, &dst);
}

SDL_Rect monochrome_widget::_texture_bounds() const
{
    SDL_Rect texture_bounds{0};
    SDL_QueryTexture(_content, nullptr, nullptr,
                               &texture_bounds.w, &texture_bounds.h);
    return texture_bounds;
}

SDL_Rect monochrome_widget::_content_bounds() const
{
    // scale the texture bounds into rendering space
    SDL_Rect const texture_bounds = _texture_bounds();
    double const rendering_scale = scale_by_height(texture_bounds, _bounds);

    SDL_Rect content_bounds(_bounds);
    content_bounds.w = static_cast<int>(texture_bounds.w * rendering_scale);

    // clamp the content bounds to be within the widget bounds
    content_bounds.x = std::max(_bounds.x, content_bounds.x);
    content_bounds.w = std::min(_bounds.w, content_bounds.w);

    return content_bounds;
}
}
