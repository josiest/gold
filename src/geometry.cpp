#include "gold/geometry.hpp"

namespace au {

double scale_by_height(SDL_Rect const & from, SDL_Rect const & to)
{
    // scale width by normalizing the rect height to the max height
    return to.h / static_cast<double>(from.h);
}
SDL_Rect clip_width(SDL_Rect const & rect, SDL_Rect const & max)
{
    SDL_Rect clipped{rect};

    // scale the rect to be clipped into the space of the max bounds
    double const scale = scale_by_height(rect, max);

    // clip the width if it's too wide
    if (clipped.w * scale > max.w) {
        clipped.w = static_cast<int>(max.w/scale);
    }
    return clipped;
}
}
