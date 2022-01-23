#include "gold/geometry.hpp"

namespace au {

double scale_by_height(SDL_Rect const & rect, SDL_Rect const & ref)
{
    // scale width by normalizing the rect height to the max height
    return ref.h / static_cast<double>(rect.h);
}
SDL_Rect clip_width(SDL_Rect const & rect, SDL_Rect const & max)
{
    SDL_Rect clipped{rect};
    double const scale = scale_by_height(rect, max);
    if (clipped.w * scale > max.w) {
        clipped.w = static_cast<int>(max.w/scale);
    }
    return clipped;
}
}
