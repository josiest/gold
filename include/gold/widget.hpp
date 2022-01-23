#pragma once

// frameworks
#include <SDL.h>
#include <SDL_ttf.h>

// data types
#include <string>

// templates data-types and concepts
#include <concepts>
#include "gold/result.hpp"

namespace au {

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

    { factory.make_text_widget(renderer, text, bounds) } ->
        std::same_as<result<iwidget *>>;
};
}
