#include "gold/color_serialization.hpp"

// data types
#include <string>

// i/o and serialization
#include <filesystem>
#include <yaml-cpp/yaml.h>

// aliases
namespace fs = std::filesystem;
using namespace std::string_literals;

namespace au {

result<color_table> load_colors(fs::path const & path)
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

    color_table table;
    for (auto const & item : colors) {
        // each entry must point to an rgb color sequence
        if (not item.second.IsSequence() or item.second.size() != 3) {
            return tl::unexpected("color definitions should have the form [r, g, b]"s);
        }
        // each value must be a scalar
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
        table[name] = color;
    }
    return table;
}
}
