#pragma once

#include <SDL.h>

namespace au {

class iwidget {
public:
    /** Get the bounds of this widget. */
    SDL_Rect bounds() const;

    /** Render this widget. */
    void render(SDL_Renderer * renderer);
};
}
