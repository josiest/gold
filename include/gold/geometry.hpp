#pragma once

// frameworks
#include <SDL.h>

namespace au {

double scale_by_height(SDL_Rect const & rect, SDL_Rect const & ref);
SDL_Rect clip_width(SDL_Rect const & rect, SDL_Rect const & max);

}
