#pragma once

// data types
#include "gold/widget.hpp"
#include <functional>
#include <cstddef> // std::size_t

namespace au {

namespace _global {
extern std::size_t clicked_button;
}

/** Register the button that was clicked. */
void click(au::iwidget * button);
/** Determine if a button was clicked this frame. */
bool is_clicked(au::iwidget * button);

/** An SDL_MouseButtonEvent listener. */
using click_fn = std::function<void(SDL_MouseButtonEvent const &)>;

/** An SDL_Event listener. */
using listener_fn = std::function<void(SDL_Event const &)>;

/** Call when creating a button to register when it clicks. */
listener_fn click_button(iwidget * button);

/** Call when adding functionality to a button. */
listener_fn on_click(iwidget * button, click_fn callback);
}
