#include "gold/button_factory.hpp"

// frameworks and interfaces
#include <SDL.h>
#include <SDL_ttf.h>
#include "gold/widget.hpp" // iwidget_factory

// data types
#include "gold/button.hpp"
#include <cstdint> // std::uint32_t

// data structures and resource handlers
#include <vector>
#include <memory> // std::unique_ptr
#include "gold/result.hpp"
#include <tl/expected.hpp> // tl::unexpected

// algorithms
#include <ranges> // ranges::transform, ranges::any_of,
                  // views:: values, views::filter

// i/o and serialization
#include <sstream>
#include <filesystem> // fs::path, fs::is_regular_file,
                      // fs::is_directory
#include <yaml-cpp/yaml.h>

// aliases
using uint = std::uint32_t;
using namespace std::string_literals;
namespace fs = std::filesystem;

namespace au {

// declare static class variables
color_table button_factory::_colors{
    {"black", SDL_Color{0, 0, 0, 0xff}},
    {"white", SDL_Color{0xff, 0xff, 0xff, 0xff}},
    {"gray", SDL_Color{0x66, 0x66, 0x66, 0xff}},
    {"light-gray", SDL_Color{0xcc, 0xcc, 0xcc, 0xff}}
};
font_table button_factory::_fonts{};

button_factory::button_factory(
        TTF_Font * font,
        std::uint32_t border_thickness, std::uint32_t padding,
        SDL_Color const & standard_color, SDL_Color const & hover_color,
        SDL_Color const & click_color, SDL_Color const & fill_color)

    : _font(font),
      _border_thickness(border_thickness), _padding(padding),
      _standard_color(standard_color), _hover_color(hover_color),
      _click_color(click_color), _fill_color(fill_color)
{
}

result<iwidget *>
button_factory::make_text_widget(SDL_Renderer * renderer,
                                 std::string const & text,
                                 SDL_Rect const & bounds)
{
    // create the button and make sure it rendered the text correctly
    auto text_widget = std::make_unique<button>(
            renderer, bounds, _border_thickness, _padding,
            _standard_color, _hover_color, _click_color, _fill_color,
            _font, text);

    if (not *text_widget) {
        return tl::unexpected("unable to render text"s);
    }

    // add the button to the owned resources and return an observer
    auto observer = dynamic_cast<iwidget *>(text_widget.get());
    _buttons.push_back(std::move(text_widget));
    return observer;
}

void button_factory::update_colors(color_table const & table)
{
    for (auto const &[name, color] : table) {
        _colors.insert_or_assign(name, color);
    }
}

void button_factory::update_fonts(font_table const & table)
{
    for (auto &[name, font] : table) {
        _fonts.insert_or_assign(name, font);
    }
}

result<button_factory> button_factory::from_file(fs::path const & path)
{
    // use error codes to force filesystem not to throw
    std::error_code ec;

    // make sure the path exists and is valid
    if (not fs::exists(path, ec) or not fs::is_regular_file(path, ec)) {
        std::stringstream message;
        if (ec) { message << ec.message(); } // an os call failed
        else if (not fs::exists(path)) { message << path << " doesn't exist"; }
        else if (not fs::is_regular_file(path)) { message << path << " isn't a file"; }
        return tl::unexpected(message.str());
    }

    // can't load a map that isn't a map
    YAML::Node config = YAML::LoadFile(path);
    if (not config.IsMap()) {
        return tl::unexpected("button config should be a yaml map"s);
    }
    // load parameters or use defaults
    auto const font = config["font"].as<std::string>("DejaVuSans");
    int const border_width = config["border-width"].as<int>(0);
    int const padding = config["padding"].as<int>(0);
    auto const standard_color = config["standard-color"].as<std::string>("black");
    auto const hover_color = config["hover-color"].as<std::string>("gray");
    auto const click_color = config["click-color"].as<std::string>("light-gray");
    auto const fill_color = config["fill-color"].as<std::string>("white");

    if (not _fonts.contains(font)) {
        return tl::unexpected("the font " + font + " hasn't been loaded");
    }
    if (not _colors.contains(standard_color)) {
        return tl::unexpected("the color " + standard_color + " hasn't been defined");
    }
    if (not _colors.contains(hover_color)) {
        return tl::unexpected("the color " + hover_color + " hasn't been defined");
    }
    if (not _colors.contains(click_color)) {
        return tl::unexpected("the color " + click_color + " hasn't been defined");
    }
    if (not _colors.contains(fill_color)) {
        return tl::unexpected("the color " + fill_color + " hasn't been defined");
    }

    return button_factory(_fonts[font], border_width, padding,
                          _colors[standard_color], _colors[hover_color],
                          _colors[click_color], _colors[fill_color]);
}
}
