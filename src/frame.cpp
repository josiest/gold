#include "gold/frame.hpp"

// frameworks and interfaces
#include <SDL.h>
#include <tl/expected.hpp>

// data types
#include <cstdint>
#include <string>

// i/o and serialization
#include <sstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

// aliases
using uint = std::uint32_t;
namespace fs = std::filesystem;
using namespace std::string_literals;

namespace au {

frame::frame(SDL_Renderer * renderer, SDL_Rect const & bounds,
             uint button_height, uint padding)
    : _renderer(renderer), _bounds(bounds),
      _button_height(static_cast<int>(button_height)),
      _padding(static_cast<int>(padding)),
      _next{bounds.x + _padding, bounds.y + _padding}
{
}
frame::frame(SDL_Renderer * renderer, int x, int y, uint w, uint h,
             uint button_height, uint padding)
    : frame(renderer, SDL_Rect{x, y, static_cast<int>(w), static_cast<int>(h)},
            button_height, padding)
{
}

void frame::render()
{
    for (auto widget : _widgets) {
        widget->render(_renderer);
    }
}

result<frame> frame::from_file(SDL_Renderer * renderer, SDL_Rect const & bounds,
                               fs::path const & path)
{
    // check that the path is valid
    std::error_code ec;
    bool const path_exists = fs::exists(path, ec);
    bool const file_is_regular = fs::is_regular_file(path, ec);
    if (ec or not path_exists or not file_is_regular) {
        std::stringstream message;
        if (ec) { // an os call failed
            message << ec.message();
        }
        else if (not path_exists) {
            message << path << " doesn't exist";
        }
        else if (not file_is_regular) {
            message << path << " isn't a file";
        }
        return tl::unexpected(message.str());
    }

    // load the file and make sure it has the right format
    YAML::Node const config = YAML::LoadFile(path);
    if (not config.IsMap()) {
        return tl::unexpected("frame config should have a yaml map format"s);
    }

    // make sure the button-height attribute exists and is a number
    if (not config["button-height"]) {
        return tl::unexpected("A button-height attribute must be specified"s);
    }
    if (not config["button-height"].IsScalar()) {
        return tl::unexpected("The button-heght attribute must be a scalar value"s);
    }
    int const height = config["button-height"].as<int>();

    // make sure that the padding attribute is a number if it exists
    if (config["padding"] and not config["padding"].IsScalar()) {
        return tl::unexpected("The padding attribute must be a scalar value"s);
    }
    int const padding = config["padding"].as<int>();

    return frame(renderer, bounds, height, padding);
}
}
