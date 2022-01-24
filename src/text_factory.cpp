#include "gold/text_factory.hpp"

// frameworks and interfaces
#include <SDL.h>
#include <SDL_ttf.h>
#include "gold/widget.hpp"

// data types
#include "gold/text_field.hpp"
#include <string>

// data structures
#include <unordered_map>

// algorithms
#include <ranges>

// i/o and serialization
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "gold/serialization.hpp"

// aliases
namespace ranges = std::ranges;
namespace fs = std::filesystem;
using namespace std::string_literals;

namespace au {

// declare static class variables
color_table text_factory::_colors{
    {"black", SDL_Color{0, 0, 0, 0xff}},
    {"white", SDL_Color{0xff, 0xff, 0xff, 0xff}},
    {"gray", SDL_Color{0x66, 0x66, 0x66, 0xff}},
    {"light-gray", SDL_Color{0xcc, 0xcc, 0xcc, 0xff}}
};
font_table text_factory::_fonts{};

text_factory::text_factory(TTF_Font * font, SDL_Color const & color)
    : _font(font), _color(color)
{
}

result<itext_widget *>
text_factory::make_text_widget(SDL_Renderer * renderer,
                               std::string const & text,
                               SDL_Rect const & bounds)
{
    // create the text field and make sure it rendered correctly
    auto text_widget = std::make_unique<text_field>(
            renderer, bounds, _font, _color, text);
    if (not *text_widget) {
        return tl::unexpected("unable to render text-field"s);
    }

    // add the text field to the owned resources and return an observer
    auto observer = dynamic_cast<itext_widget *>(text_widget.get());
    _texts.push_back(std::move(text_widget));
    return observer;
}

void text_factory::update_colors(color_table const & table)
{
    ranges::for_each(table, [](auto const & item) {
        _colors.insert_or_assign(item.first, item.second);
    });
}

void text_factory::update_fonts(font_table const & table)
{
    ranges::for_each(table, [](auto const & item) {
        _fonts.insert_or_assign(item.first, item.second);
    });
}

result<text_factory> text_factory::from_file(fs::path const & path)
{
    // use error codes to force filesystem not to throw errors
    std::error_code ec;

    // make sure the file is valid
    bool const file_exists = fs::exists(path, ec);
    bool const file_is_regular = fs::is_regular_file(path, ec);

    if (ec or not file_exists or not file_is_regular) {
        std::stringstream message;
        if (ec) { // an os call failed
            message << ec.message();
        }
        else if (not file_exists) {
            message << path << " doesn't exist";
        }
        else if (not file_is_regular) {
            message << path << " isn't a file";
        }
        return tl::unexpected(message.str());
    }

    // can't load a map that isn't a map
    YAML::Node config = YAML::LoadFile(path);
    if (not config.IsMap()) {
        return tl::unexpected("text config should be a yaml map"s);
    }

    // load parameters or use defaults
    auto const font = config["font"].as<std::string>("DejaVuSans");
    auto const text_color = config["text-color"].as<std::string>("black");

    if (not _fonts.contains(font)) {
        return tl::unexpected("the font " + font + " hasn't been loaded");
    }
    if (not _colors.contains(text_color)) {
        return tl::unexpected("the color " + text_color + " hasn't been defined");
    }

    return text_factory(_fonts[font], _colors[text_color]);
}
}
