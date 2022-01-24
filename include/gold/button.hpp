#pragma once

// frameworks
#include <SDL.h>

// resource handlers
#include "gold/serialization.hpp" // unqiue_texture

// data types
#include "gold/widget.hpp"
#include <cstdint>

namespace au {

class button : public itext_widget {
public:
    button() = delete;

    button(SDL_Renderer * renderer, SDL_Rect const & bounds,
           std::uint32_t border_thickness, std::uint32_t padding,

           SDL_Color const & standard_color, SDL_Color const & hover_color,
           SDL_Color const & click_color, SDL_Color const & fill_color,

           TTF_Font * font, std::string const & text);

    inline bool operator !() const { return not _content; }

    // interface methods
    inline SDL_Rect bounds() const { return _bounds; }
    void render(SDL_Renderer * renderer);

    inline std::size_t id() const { return _id; }
    inline bool is_active() const { return _active; }
    void activate() { _active = true; }
    void deactivate() { _active = false; }

    inline std::string get_text() const { return _text; }
    void set_text(std::string const & text);
private:
    static std::size_t constexpr _seed = 7878831530993720721u;
    static std::size_t _next_id;

    std::size_t const _id;
    SDL_Renderer * _renderer;
    SDL_Rect _bounds;
    int _border_thickness;
    int _padding;

    SDL_Color _standard_color;
    SDL_Color _hover_color;
    SDL_Color _click_color;
    SDL_Color _fill_color;

    TTF_Font * _font;
    std::string _text;
    unique_texture _content;
    bool _active;

    // render text using the object's font
    SDL_Texture * _render_text(std::string const & text);

    // get the bounding rect of the button inside the border
    SDL_Rect _inner_bounds() const;

    // get the maximum bounding rect for the inner content
    SDL_Rect _max_content_bounds() const;

    // get the bounding rect to draw the texture to
    SDL_Rect _content_bounds() const;

    // get the raw bounding rect of the texture
    SDL_Rect _texture_bounds() const;

    // determine if the mouse is in the bounds of this button
    bool _mouse_in_bounds() const;

    // determine of the mouse was clicked
    bool _mouse_clicked() const;
};
}
