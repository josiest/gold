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

auto add_to_counter(au::iwidget * button, au::itext_widget * field,
                    int & counter, int amt)
{
    return [button, field, amt, &counter](SDL_Event const & event) {

        // don't do anything if the button clicked isn't active
        if (not button->is_active()) { return; }

        SDL_Rect const bounds = button->bounds();
        SDL_Point const mouse{event.button.x, event.button.y};

        if (au::within_closed_bounds(mouse, bounds)) {
            counter += amt;
            field->set_text(std::to_string(counter) + " clicks");
        }
    };
}

auto set_button_text(au::iwidget * click_button, au::itext_widget * text_button,
                     std::string const original)
{
    return [click_button, text_button, &original](SDL_Event const & event) {

        // don't do anything if the button clicked isn't active
        if (not click_button->is_active()) { return; }

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

auto activate_frame(au::iwidget * click_button, au::frame & frame, bool activate)
{
    return [click_button, activate, &frame](SDL_Event const & event) {

        // don't do anything if button is deactivated
        if (not click_button->is_active()) { return; }

        SDL_Rect const bounds = click_button->bounds();
        SDL_Point const mouse{event.button.x, event.button.y};

        if (au::within_closed_bounds(mouse, bounds)) {
            if (activate) { frame.activate(); }
            else { frame.deactivate(); }
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
    uint const screen_height = 260;
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
    auto button_frame =
        au::frame::from_file(window, config_dir/"button-frame.yaml");
    if (not button_frame) {
        std::cout << button_frame.error() << std::endl;
        return EXIT_FAILURE;
    }

    // add some buttons to the frame
    auto simple_button = button_frame->produce_text_widget(
            *button_maker, "Click Me!");
    if (not simple_button) {
        std::cout << simple_button.error() << std::endl;
        return EXIT_FAILURE;
    }

    std::string const another_text = "Another Button!";
    auto another = button_frame->produce_text_widget(
            *button_maker, another_text);
    if (not another) {
        std::cout << another.error() << std::endl;
        return EXIT_FAILURE;
    }

    auto deactivate_button = button_frame->produce_text_widget(
            *button_maker, "Deactivate");
    if (not deactivate_button) {
        std::cout << deactivate_button.error() << std::endl;
        return EXIT_FAILURE;
    }

    // create a factory object to create text fields
    au::text_factory::update_fonts(au::observe_fonts(*fonts));
    au::text_factory::update_colors(*colors);
    auto text_maker = au::text_factory::from_file(config_dir/"text.yaml");
    if (not text_maker) {
        std::cout << text_maker.error() << std::endl;
        return EXIT_FAILURE;
    }

    // create a frame to render the text field in
    auto text_frame =
        au::frame::from_file(window, config_dir/"text-frame.yaml");
    if (not text_frame) {
        std::cout << text_frame.error() << std::endl;
        return EXIT_FAILURE;
    }

    // add the counter text to the frame
    auto expected_counter =
        text_frame->produce_text_widget(*text_maker, "Click counter");
    if (not expected_counter) {
        std::cout << expected_counter.error() << std::endl;
        return EXIT_FAILURE;
    }
    au::itext_widget * counter_field = *expected_counter;

    // add the activate button to the text frame
    auto activate_button = text_frame->produce_text_widget(
            *button_maker, "Activate");
    if (not activate_button) {
        std::cout << activate_button.error() << std::endl;
        return EXIT_FAILURE;
    }

    // link some call-backs to the buttons

    // increase the counter every time the simple button is clicked
    int counter = 0;
    auto add_one = add_to_counter(dynamic_cast<au::iwidget *>(*simple_button),
                                  counter_field, counter, 1);
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, add_one);

    // flip the rendered text every time another button is clicked
    auto alakazam = set_button_text(dynamic_cast<au::iwidget *>(*another),
                                    *another, another_text);
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, alakazam);

    // deactivate the button frame when the deactivate button is clicked
    auto set_active =
        activate_frame(dynamic_cast<au::iwidget *>(*activate_button),
                       *button_frame, true);
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, set_active);

    // activate the button frame when the activate button is clicked
    auto set_not_active =
        activate_frame(dynamic_cast<au::iwidget *>(*deactivate_button),
                       *button_frame, false);
    events.subscribe_functor(SDL_MOUSEBUTTONDOWN, set_not_active);

    while (not ion::input::has_quit()) {
        events.process_queue();

        // clear the screen white
        SDL_SetRenderDrawColor(window, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(window);

        // draw all the widgets associated with the frame
        button_frame->render();
        text_frame->render();
        SDL_RenderPresent(window);
    }
    return EXIT_SUCCESS;
}
