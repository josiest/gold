#pragma once

#include <tl/expected.hpp>
#include <SDL.h>
#include <string>
#include <concepts>

namespace au {

class iwidget {
public:
    /** Get the bounds of this widget. */
    virtual SDL_Rect bounds() const = 0;

    /** Render this widget. */
    virtual void render(SDL_Renderer * renderer) = 0;

    virtual ~iwidget() {}
};

template<class factory_t, typename error_t = std::string>
concept widget_factory =
    requires(factory_t & factory, SDL_Renderer * renderer,
             std::string const & text, SDL_Rect const & bounds) {

    { factory.make_widget(renderer, text, bounds) } ->
        std::same_as<tl::expected<iwidget *, error_t>>;
};
}
