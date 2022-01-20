#pragma once

// frameworks and interfaces
#include <SDL.h>
#include <concepts>
#include <tl/expected.hpp>

// data types
#include <string>
#include <memory> // std::unique_ptr

namespace au {

// destroy various sdl resources
struct sdl_deleter {
    void operator()(SDL_Texture * texture) { SDL_DestroyTexture(texture); }
};

// aliases
using unique_texture = std::unique_ptr<SDL_Texture, sdl_deleter>;
template<typename expected_t>
using result = tl::expected<expected_t, std::string>;

class iwidget {
public:
    /** Get the bounds of this widget. */
    virtual SDL_Rect bounds() const = 0;

    /** Render this widget. */
    virtual void render(SDL_Renderer * renderer) = 0;

    virtual ~iwidget() {}
};

template<class factory_t>
concept widget_factory =

    requires(factory_t & factory, SDL_Renderer * renderer,
             std::string const & text, SDL_Rect const & bounds) {

    { factory.make_widget(renderer, text, bounds) } ->
        std::same_as<result<iwidget *>>;
};
}
