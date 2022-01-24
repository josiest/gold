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

#include <ranges>

// aliases
using uint = std::uint32_t;
namespace fs = std::filesystem;
namespace ranges = std::ranges;
using namespace std::string_literals;

namespace au {

frame::frame(SDL_Renderer * renderer, SDL_Rect const & bounds,
             uint button_height, uint padding)
    : _renderer(renderer), _bounds(bounds),
      _button_height(static_cast<int>(button_height)),
      _padding(static_cast<int>(padding)),
      _next{bounds.x + _padding, bounds.y + _padding},
      _active(true)
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

void frame::activate()
{
    for (auto widget : _widgets) {
        widget->activate();
    }
}

void frame::deactivate()
{
    for (auto widget : _widgets) {
        widget->deactivate();
    }
}

result<frame> frame::from_file(SDL_Renderer * renderer, fs::path const & path)
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

    // make sure the appropriate attribute exists in the right format
    if (not config["widget-height"]) {
        return tl::unexpected("A widget-height attribute must be specified"s);
    }
    if (not config["widget-height"].IsScalar()) {
        return tl::unexpected("The widget-height attribute must be a scalar value"s);
    }
    int const height = config["widget-height"].as<int>();

    if (not config["x"]) {
        return tl::unexpected("An x attribute must be specified"s);
    }
    if (not config["x"].IsScalar()) {
        return tl::unexpected("The x attribute must be a scalar value"s);
    }
    int const x = config["x"].as<int>();

    if (not config["y"]) {
        return tl::unexpected("A y attribute must be specified"s);
    }
    if (not config["y"].IsScalar()) {
        return tl::unexpected("The y attribute must be a scalar value"s);
    }
    int const y = config["y"].as<int>();

    if (not config["width"]) {
        return tl::unexpected("A width attribute must be specified"s);
    }
    if (not config["width"].IsScalar()) {
        return tl::unexpected("The width attribute must be a scalar value"s);
    }
    int const w = config["width"].as<int>();

    if (not config["height"]) {
        return tl::unexpected("A height attribute must be specified"s);
    }
    if (not config["height"].IsScalar()) {
        return tl::unexpected("The height attribute must be a scalar value"s);
    }
    int const h = config["height"].as<int>();

    // make sure that the padding attribute is a number if it exists
    if (config["padding"] and not config["padding"].IsScalar()) {
        return tl::unexpected("The padding attribute must be a scalar value"s);
    }
    int const padding = config["padding"].as<int>();

    return frame(renderer, SDL_Rect{x, y, w, h}, height, padding);
}
}
