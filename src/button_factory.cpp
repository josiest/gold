#include "gold/button_factory.hpp"

// frameworks and interfaces
#include <SDL.h>
#include <SDL_ttf.h>
#include <yaml-cpp/yaml.h>
#include <filesystem>   // fs::path, fs::is_regular_file
#include "gold/widget.hpp" // iwidget_factory

// data structures and resource handlers
#include <vector>
#include <memory> // std::unique_ptr
#include <tl/expected.hpp>

// data types
#include "gold/button.hpp"
#include <cstdint> // std::uint32_t

// i/o
#include <sstream>

// aliases
using uint = std::uint32_t;
using namespace std::string_literals;
namespace fs = std::filesystem;

namespace au {

// declare static class variables
std::unordered_map<std::string, SDL_Color> button_factory::_colors;

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
button_factory::make_widget(SDL_Renderer * renderer, std::string const & text,
                            SDL_Rect const & bounds)
{
    // render the text white to get pure color modulation capability
    static SDL_Color const white{0xff, 0xff, 0xff, 0xff};

    // if the text has already been rendered, get its observer
    SDL_Texture * rendered_text = nullptr;
    if (_texts.contains(text)) {
        rendered_text = _texts[text].get();
    }
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
    return _buttons.emplace_back(new button(
                bounds, _border_thickness, _padding,
                _standard_color, _hover_color, _click_color, _fill_color,
                rendered_text
                )).get();
}

result<bool>
button_factory::load_colors(fs::path const & path)
{
    // make sure the path exists and is a file
    if (not fs::exists(path) or fs::is_regular_file(path)) {
        std::stringstream message;
        if (not fs::exists(path)) message << path << " doesn't exist";
        if (not fs::is_regular_file(path)) message << path << " isn't a file";
        return tl::unexpected(message.str());
    }

    YAML::Node colors = YAML::LoadFile(path);

    // can't load a map that isn't a map
    if (not colors.IsMap())
        return tl::unexpected("colors should be defined as a yaml map"s);

    for (auto const & item : colors) {
        // each entry must point to an rgb color sequence
        if (not item.second.IsSequence() or item.second.size() != 3)
            return tl::unexpected("color definitions should have the form [r, g, b]"s);

        // each value mus be a scalar
        for (auto const & val : item.second)
            if (not val.IsScalar())
                return tl::unexpected("color definition values should be scalar"s);
    }
    _colors = colors.as<color_table>();
    return true;
}
}
