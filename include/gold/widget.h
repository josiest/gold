#pragma once

#include <SDL.h>
#include <string>

namespace au {

class iwidget {
public:
    /** Get the bounds of this widget. */
    virtual SDL_Rect bounds() const = 0;

    /** Render this widget. */
    virtual void render(SDL_Renderer * renderer) = 0;

    virtual ~iwidget() {}
};

class iwidget_factory {
public:
    /** Make a widget with text */
    virtual iwidget * make(SDL_Renderer * renderer, std::string const & text,
                           SDL_Rect const & bounds) = 0;

    virtual ~iwidget_factory() {}
};
}
