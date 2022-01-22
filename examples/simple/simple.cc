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

    // directory constants
    auto const asset_dir = fs::absolute("../assets");
    auto const config_dir = asset_dir/"config";
    auto const font_dir = asset_dir/"fonts";

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
    uint const screen_width = 800;
    uint const screen_height = 600;
    auto window = ion::hardware_renderer::basic_window(
            "Simple Example", screen_width, screen_height);
    if (not window) {
        std::cout << window.get_error() << std::endl;
        return EXIT_FAILURE;
    }

    // define some colors and fonts
    auto const colors_did_load =
        au::button_factory::load_colors(config_dir/"colors.yaml");

    if (not colors_did_load) {
        std::cout << colors_did_load.error() << std::endl;
        return EXIT_FAILURE;
    }
    auto fonts = au::button_factory::load_all_fonts(font_dir);
    if (not fonts) {
        std::cout << fonts.error() << std::endl;
        return EXIT_FAILURE;
    }

    // create a button factory and make a button
    auto button_maker = au::button_factory::from_file(config_dir/"button.yaml");
    if (not button_maker) {
        std::cout << button_maker.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::frame button_frame(window, 0, 0, 3*screen_width/5, screen_height, 100, 50);
    auto expected_button = button_frame.produce_widget(*button_maker, "Click Me!");

    if (not expected_button) {
        std::cout << expected_button.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::iwidget * simple_button = *expected_button;

    expected_button = button_frame.produce_widget(*button_maker, "Another!");
    if (not expected_button) {
        std::cout << expected_button.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::iwidget * another = *expected_button;

    // link sdl click event to this button
    std::string const simple_message = "Clicked!";
    auto print_clicked = print_message_fxn(simple_button, simple_message);
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, print_clicked);

    std::string const another_message = "Another clicked!";
    auto print_another = print_message_fxn(another, another_message);
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, print_another);

    while (not ion::input::has_quit()) {
        events.process_queue();

        // clear the screen white
        SDL_SetRenderDrawColor(window, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(window);

        // draw the button and update the screen
        simple_button->render(window);
        another->render(window);
        SDL_RenderPresent(window);
    }
    return EXIT_SUCCESS;
}
