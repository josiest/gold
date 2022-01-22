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
#include <tl/expected.hpp>

// algorithms
#include <ranges>

// i/o and serialization
#include <sstream>
#include <filesystem>   // fs::path, fs::is_regular_file
#include <yaml-cpp/yaml.h>
#include "gold/yaml_colors.hpp"

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
    // render the text white to get pure color modulation capability
    static SDL_Color const white{0xff, 0xff, 0xff, 0xff};

    // if the text has already been rendered, get its observer
    SDL_Texture * rendered_text = nullptr;
    if (_texts.contains(text)) {
        rendered_text = _texts[text].get();
    }
    else {
        // otherwise render it, convert the surface to a texture
        // and add it to the resource manager
        SDL_Surface * text_surface =
            TTF_RenderText_Solid(_font, text.c_str(), white);
        if (not text_surface) {
            return tl::unexpected(TTF_GetError());
        }

        rendered_text = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_FreeSurface(text_surface);
        if (not rendered_text) {
            return tl::unexpected(SDL_GetError());
        }

        // two resource managers to add to
        // 1. the texture resources, since buttons don't own textures
        // 2. the button resources
        _texts[text] = unique_texture(rendered_text, sdl_deleter{});
    }
    return _buttons.emplace_back(new button(
                bounds, _border_thickness, _padding,
                _standard_color, _hover_color, _click_color, _fill_color,
                rendered_text
                )).get();
}

result<bool>
button_factory::load_colors(fs::path const & path)
{
    std::error_code ec; // using error codes tells filesystem not to throw

    // make sure the path exists and is a file
    if (not fs::exists(path, ec) or not fs::is_regular_file(path, ec)) {
        std::stringstream message;
        if (ec) { // an os call failed
            message << ec.message();
        }
        else if (not fs::exists(path)) {
            message << path << "doesn't exist";
        }
        else if (not fs::is_regular_file(path)) {
            message << path << "isn't a file";
        }
        return tl::unexpected(message.str());
    }

    YAML::Node colors = YAML::LoadFile(path);

    // can't load a map that isn't a map
    if (not colors.IsMap()) {
        return tl::unexpected("colors should be defined as a yaml map"s);
    }
    for (auto const & item : colors) {
        // each entry must point to an rgb color sequence
        if (not item.second.IsSequence() or item.second.size() != 3) {
            return tl::unexpected("color definitions should have the form [r, g, b]"s);
        }
        // each value mus be a scalar
        for (auto const & val : item.second) {
            if (not val.IsScalar()) {
                return tl::unexpected("color definition values should be scalar"s);
            }
        }
        std::string name{""};
        if (not YAML::convert<std::string>::decode(item.first, name)) {
            return tl::unexpected("couldn't decode color name!"s);
        }
        SDL_Color color{0};
        if (not YAML::convert<SDL_Color>::decode(item.second, color)) {
            return tl::unexpected("coldn't decode color!"s);
        }
        _colors[name] = color;
    }
    return true;
}

// determine if a path is a valid font file
auto _is_font_fxn(std::error_code & ec) {
    return [&ec](fs::path const & path) {
        return fs::is_regular_file(path, ec) and path.extension() == ".ttf";
    };
}
// load a font-name, font-pointer pair from a font file
font_table::value_type _load_as_pair(fs::path const & path) {
    return std::make_pair(path.stem(), TTF_OpenFont(path.c_str(), 100));
}
// create a unique font pointer from a name-pointer pair
unique_font _as_unique_font(TTF_Font * font) {
    return unique_font(font, sdl_deleter{});
}
// determine if a font pointer is null from a name-pointer pair
bool _font_is_null(TTF_Font const * font) {
    return font == nullptr;
}

result<std::vector<unique_font>>
button_factory::load_all_fonts(fs::path const & dir)
{
    namespace views = std::views;
    namespace ranges = std::ranges;

    std::error_code ec; // using error codes tells filesystem not to throw

    // make sure the path exists and is a valid directory
    if (not fs::exists(dir, ec) or not fs::is_directory(dir, ec)) {
        std::stringstream message;
        if (ec) { message << ec.message(); } // an os call failed
        else if (not fs::exists(dir)) {
            message << dir << "doesn't exist";
        }
        else if (not fs::is_directory(dir)) {
            message << dir << "isn't a directory";
        }
        return tl::unexpected(message.str());
    }

    // recursively get all paths in directory
    std::vector<fs::path> paths(fs::recursive_directory_iterator(dir, ec), {});

    // filter only font files and load them as name-font pairs
    auto is_font = _is_font_fxn(ec);
    auto font_files = paths | views::filter(is_font);
    auto into_table = std::inserter(_fonts, _fonts.end());
    ranges::transform(font_files, into_table, &_load_as_pair);

    // put all the successfully loaded fonts into a vector of unique pointers
    // the font resources are freed automatically if returning unexpected
    // otherwise this is the expected result
    std::vector<unique_font> font_handle;
    auto into_handle = std::back_inserter(font_handle);
    auto ttf_fonts = _fonts | views::values;
    ranges::transform(ttf_fonts, into_handle, &_as_unique_font);

    // abort if any os calls failed in the process
    // or if some fonts couldn't be loaded
    bool const loading_failed = ranges::any_of(ttf_fonts, &_font_is_null);
    if (ec or loading_failed) {
        std::string message;
        if (ec) { message = ec.message(); }
        if (loading_failed) { message = TTF_GetError(); }
        _fonts.clear(); // clean up
        return tl::unexpected(message);
    }

    return font_handle;
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
