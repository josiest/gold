#include "gold/text_field.hpp"

// frameworks
#include <SDL.h>

// math
#include "gold/geometry.hpp"

// resource_handlers
#include "gold/serialization.hpp"

namespace au {

text_field::text_field(SDL_Renderer * renderer, SDL_Rect const & bounds,
                       TTF_Font * font, SDL_Color const & text_color,
                       std::string const & text)

    : _renderer(renderer), _bounds(bounds), _font(font),
      _text_color(text_color), _content(_render_text(text), sdl_deleter{})
{
}

void text_field::render(SDL_Renderer * renderer)
{
    // compute the src and dest rects
    SDL_Rect const src = clip_width(_texture_bounds(), _bounds);
    SDL_Rect const dst = _content_bounds();

    // change the color mod and render
    SDL_SetTextureColorMod(
            _content.get(), _text_color.r, _text_color.g, _text_color.b);
    SDL_RenderCopy(renderer, _content.get(), &src, &dst);
}

SDL_Texture * text_field::_render_text(std::string const & text)
{
    // make sure the font and renderer exist
    if (not _font or not _renderer) { return nullptr; }

    // render the text and convert it to a texture
    SDL_Color const white{0xff, 0xff, 0xff, 0xff};
    SDL_Surface * text_surface = TTF_RenderText_Solid(_font, text.c_str(), white);
    if (not text_surface) { return nullptr; }
    SDL_Texture * rendered_text =
        SDL_CreateTextureFromSurface(_renderer, text_surface);

    // discard any unused resources
    SDL_FreeSurface(text_surface);
    return rendered_text;
}

SDL_Rect text_field::_texture_bounds() const
{
    SDL_Rect texture_bounds{0};
    SDL_QueryTexture(_content.get(), nullptr, nullptr,
                                     &texture_bounds.w, &texture_bounds.h);
    return texture_bounds;
}

SDL_Rect text_field::_content_bounds() const
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
