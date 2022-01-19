// frameworks
#include <SDL.h>
#include <SDL_ttf.h>
#include <gold/gold.h>
#include <ion/ion.hpp>

// constants
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE

// data types
#include <cstdint> // std::uint32_t
#include <tl/expected.hpp>

// i/o
#include <iostream>
#include <filesystem> // fs::path, fs::absolute

// aliases 
namespace fs = std::filesystem;
using uint = std::uint32_t;

int main()
{
    // create the sdl event-handler: quit when sdl's quit event is triggered
    ion::event_system events;
    events.subscribe(SDL_QUIT, &ion::input::quit_on_event);

    // initialize sdl and ttf
    ion::sdl_context sdl;
    sdl.init_ttf();
    if (not sdl) {
        std::cout << sdl.get_error() << std::endl;
        return EXIT_FAILURE;
    }

    // create a basic window, specifying the title and dimensions
    auto window = ion::hardware_renderer::basic_window("Simple Example", 800, 600);
    if (not window) {
        std::cout << window.get_error() << std::endl;
        return EXIT_FAILURE;
    }

    // Load a font
    fs::path const font_path = fs::absolute("../assets/fonts/DejaVuSans.ttf");
    ion::font dejavu_sans = ion::font::from_file(font_path, 100);
    if (not dejavu_sans) {
        std::cout << dejavu_sans.get_error() << std::endl;
        return EXIT_FAILURE;
    }

    // define some colors
    SDL_Color const white{0xff, 0xff, 0xff, 0xff};
    SDL_Color const charcoal{0x40, 0x40, 0x40, 0xff};
    SDL_Color const light_blue{0x33, 0x99, 0xff, 0xff};
    SDL_Color const lighter_blue{0x99, 0xcc, 0xff, 0xff};

    // create the button factory
    uint const border_width = 10;
    uint const padding = 5;
    au::button_bag buttons(
            dejavu_sans, border_width, padding,
            charcoal, light_blue, lighter_blue, white
            );

    // create a button
    auto expected_button = buttons.make(
            window, "Click Me!", SDL_Rect{50, 50, 300, 100}
            );
    if (not expected_button) {
        std::cout << expected_button.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::iwidget * simple_button = *expected_button;

    while (not ion::input::has_quit()) {
        events.process_queue();

        // clear the screen white
        SDL_SetRenderDrawColor(window, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(window);

        // draw the button and update the screen
        simple_button->render(window);
        SDL_RenderPresent(window);
    }
    return EXIT_SUCCESS;
}
