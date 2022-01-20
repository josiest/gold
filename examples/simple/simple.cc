// frameworks
#include <SDL.h>
#include <SDL_ttf.h>
#include <gold/gold.hpp>
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

auto print_message_fxn(au::iwidget * widget, std::string const & message)
{
    return [widget, &message](SDL_Event const & event) {

        SDL_Rect const bounds = widget->bounds();
        int const x = event.button.x;
        int const y = event.button.y;

        if (x >= bounds.x && x <= bounds.x + bounds.w
                && y >= bounds.y && y <= bounds.y + bounds.h) {

            std::cout << message << std::endl;
        }
    };
}

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

    // directory constants
    auto const asset_dir = fs::absolute("../assets");
    auto const font_dir = asset_dir/"fonts";
    auto const config_dir = asset_dir/"config";

    // Load a font
    ion::font dejavu_sans = ion::font::from_file(font_dir/"DejaVuSans.ttf", 100);
    if (not dejavu_sans) {
        std::cout << dejavu_sans.get_error() << std::endl;
        return EXIT_FAILURE;
    }

    // define some colors
    au::button_factory::load_colors(config_dir/"colors.yaml");
    SDL_Color const white{0xff, 0xff, 0xff, 0xff};
    SDL_Color const charcoal{0x40, 0x40, 0x40, 0xff};
    SDL_Color const light_blue{0x33, 0x99, 0xff, 0xff};
    SDL_Color const lighter_blue{0x99, 0xcc, 0xff, 0xff};

    // create the button factory
    uint const border_width = 10;
    uint const padding = 5;
    au::button_factory buttons(
            dejavu_sans, border_width, padding,
            charcoal, light_blue, lighter_blue, white
            );

    // create a button
    auto expected_button = buttons.make_widget(
            window, "Click Me!", SDL_Rect{50, 50, 300, 100}
            );
    if (not expected_button) {
        std::cout << expected_button.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::iwidget * simple_button = *expected_button;

    // link sdl click event to this button
    auto print_clicked = print_message_fxn(simple_button, "clicked!");
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, print_clicked);

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
