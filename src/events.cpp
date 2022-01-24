#include "gold/events.hpp"

// frameworks
#include <SDL.h>

// data types
#include <cstddef> // std::size_t
#include "gold/widget.hpp"

// math
#include "gold/geometry.hpp"

namespace au {

namespace _global {
std::size_t clicked_button = 0;
}

void click(iwidget * button)
{
    if (not button) {
        _global::clicked_button = 0;
    }
    else {
        _global::clicked_button = button->id();
    }
}
bool is_clicked(iwidget * button)
{
    return button->id() == _global::clicked_button;
}

listener_fn click_button(iwidget * button)
{
    return [button](SDL_Event const & event) {
        // don't register click if the button isn't active
        if (not button->is_active()) { return; }

        // get the relevant event information
        SDL_Rect const bounds = button->bounds();
        SDL_Point const mouse{event.button.x, event.button.y};

        // register the click if the mouse is in bounds
        if (within_closed_bounds(mouse, bounds)) { click(button); }
    };
}

listener_fn on_click(iwidget * button, click_fn callback)
{
    return [button, callback](SDL_Event const & event) {
        if (not button->is_active() or not is_clicked(button)) { return; }

        // get the event information
        SDL_Rect const bounds = button->bounds();
        SDL_Point const mouse{event.button.x, event.button.y};

        // call the function
        if (within_closed_bounds(mouse, bounds)) { callback(event.button); }
    };
}
}
