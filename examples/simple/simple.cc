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
#include <string>

// i/o
#include <iostream>
#include <filesystem> // fs::path, fs::absolute

// aliases 
namespace fs = std::filesystem;
using uint = std::uint32_t;

auto add_to_counter(au::iwidget * button, au::text_field & field,
                    int & counter, int amt)
{
    return [button, amt, &field, &counter](SDL_Event const & event) {

        SDL_Rect const bounds = button->bounds();
        SDL_Point const mouse{event.button.x, event.button.y};

        if (au::within_closed_bounds(mouse, bounds)) {
            counter += amt;
            field.set_text(std::to_string(counter) + " clicks");
        }
    };
}

auto set_button_text(au::iwidget * click_button, au::itext_widget * text_button,
                     std::string const original)
{
    return [click_button, text_button, &original](SDL_Event const & event) {

        SDL_Rect const bounds = click_button->bounds();
        SDL_Point const mouse{event.button.x, event.button.y};

        std::string const alakazam = "Alakazam!";
        if (au::within_closed_bounds(mouse, bounds)) {

            // change the button text to alkazam if it isn't already
            if (text_button->get_text() != alakazam) {
                text_button->set_text(alakazam);
            }
            // otherwise change back to original text
            else {
                text_button->set_text(original);
            }
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
    auto colors = au::load_colors(config_dir/"colors.yaml");
    if (not colors) {
        std::cout << colors.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::button_factory::update_colors(*colors);

    auto fonts = au::load_all_fonts(font_dir);
    if (not fonts) {
        std::cout << fonts.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::button_factory::update_fonts(au::observe_fonts(*fonts));

    // create a button factory to specify how to make buttons
    auto button_maker = au::button_factory::from_file(config_dir/"button.yaml");
    if (not button_maker) {
        std::cout << button_maker.error() << std::endl;
        return EXIT_FAILURE;
    }

    // create a frame to render the buttons in
    SDL_Rect const frame_bounds{0, 0, 3*screen_width/7, screen_height};
    auto button_frame = au::frame::from_file(
            window, frame_bounds, config_dir/"frame.yaml");
    if (not button_frame) {
        std::cout << button_frame.error() << std::endl;
        return EXIT_FAILURE;
    }
    auto expected_button = button_frame->produce_text_widget(*button_maker, "Click Me!");

    // add some buttons to the frame
    if (not expected_button) {
        std::cout << expected_button.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::itext_widget * simple_button = *expected_button;

    std::string const another_text = "Another Button!";
    expected_button = button_frame->produce_text_widget(*button_maker, another_text);
    if (not expected_button) {
        std::cout << expected_button.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::itext_widget * another = *expected_button;

    // get references to the font and color used for the text field
    auto font_search = fonts->find("DejaVuSans");
    if (font_search == fonts->end()) {
        std::cout << "DejaVu sans hasn't been loaded!" << std::endl;
        return EXIT_FAILURE;
    }
    TTF_Font * dejavu_sans = font_search->second.get();

    auto const charcoal_search = colors->find("charcoal");
    if (charcoal_search == colors->end()) {
        std::cout << "The color charcoal hasn't been defined!" << std::endl;
        return EXIT_FAILURE;
    }
    SDL_Color const charcoal = charcoal_search->second;

    // create the text field
    SDL_Rect const counter_bounds{200, 200, 400, 60};
    std::string const text = "Click counter";
    au::text_field counter_field(
            window, counter_bounds, dejavu_sans, charcoal, text);

    // link some call-backs to the buttons
    int counter = 0;
    auto add_one = add_to_counter(simple_button, counter_field, counter, 1);
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, add_one);

    auto alakazam = set_button_text(
            dynamic_cast<au::iwidget *>(another), another, another_text);
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, alakazam);

    while (not ion::input::has_quit()) {
        events.process_queue();

        // clear the screen white
        SDL_SetRenderDrawColor(window, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(window);

        // draw all the widgets associated with the frame
        button_frame->render();
        counter_field.render(window);
        SDL_RenderPresent(window);
    }
    return EXIT_SUCCESS;
}
