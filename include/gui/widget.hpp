#pragma once

#include <SDL.h>

namespace gui {

class iwidget {
public:
    /** Get the bounds of this widget. */
    SDL_Rect bounds() const;

    /** Render this widget. */
    void render(SDL_Renderer * renderer);
};
}
