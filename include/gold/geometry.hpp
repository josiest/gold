#pragma once

// frameworks
#include <SDL.h>

namespace au {

/** Determine if a point is within closed bounds of a rect. */
bool within_closed_bounds(SDL_Point const & p, SDL_Rect const & rect);

/** Calculate the height ratio of one rect to another.
 *
 * \param from the rect that would be scaled from
 * \param to the rect that would be scaled to
 */
double scale_by_height(SDL_Rect const & from, SDL_Rect const & to);

/** Clip a rectangle so it fits within a maximum bounding rect.
 *
 * \param rect the rect to clip
 * \param max the maximum bounding rect
 *
 * In particular, when the rectangle is scaled to the space of the maximum
 * bounds, relative to the same height, the width shouldn't be longer than
 * the maximum bounds.
 */
SDL_Rect clip_width(SDL_Rect const & rect, SDL_Rect const & max);

}
