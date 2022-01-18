// frameworks
#include <SDL.h>
#include <SDL_ttf.h>
#include <gold/gold.h>
#include <ion/ion.hpp>

// constants
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE

// i/o
#include <iostream>
#include <filesystem> // fs::path, fs::absolute

// aliases 
namespace fs = std::filesystem;

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

    // Create the text for the button
    SDL_Color const black{0, 0, 0, 0xff};
    ion::texture rendered_text =
        dejavu_sans.render_text(window, "Click Me!", black);
    if (not rendered_text) {
        std::cout << rendered_text.get_error() << std::endl;
        return EXIT_FAILURE;
    }

    // create a button
    SDL_Rect const button_bounds{50, 50, 300, 100};
    SDL_Color const white{0xff, 0xff, 0xff, 0xff};
    au::button simple_button(button_bounds, black, white, 10,
                             rendered_text, 5, 5);

    bool has_quit = false;
    while (not has_quit) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                has_quit = true;
            }
        }

        // clear the screen white
        SDL_SetRenderDrawColor(window, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(window);

        // draw the button and update the screen
        simple_button.render(window);
        SDL_RenderPresent(window);
    }
    return EXIT_SUCCESS;
}
