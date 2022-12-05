#pragma once
#include <SDL2/SDL.h>
#include <cstdint>

namespace ion {

/** \brief A simple timer */
class timer {
public:
    /** \brief Start a new timer. */
    inline timer() : start{ SDL_GetTicks() } {}

    /** \brief Count the number of milliseconds since the timer started. */
    inline std::uint32_t ticks() const { return SDL_GetTicks() - start; }

    /** \brief Restart the timer. */
    inline void reset() { start = SDL_GetTicks(); }
private:
    // the global time in milliseconds when the timer was started
    std::uint32_t start;
};
}
