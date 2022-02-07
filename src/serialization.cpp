#include "gold/serialization.hpp"

// frameworks
#include <SDL.h>
#include <SDL_ttf.h>

// data types
#include <string>

// algorithms
#include <ranges>
#include <algorithm>

// i/o and serialization
#include <sstream>
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

// determine if a path is a valid font file
auto _is_font_fxn(std::error_code & ec) {
    return [&ec](fs::path const & path) {
        return fs::is_regular_file(path, ec) and path.extension() == ".ttf";
    };
}
// load a font-name, font-pointer pair from a font file
auto _load_as_pair_with_size(uint resolution) {
    return [resolution](fs::path const & path) {
        int const size = static_cast<int>(resolution);
        auto font = unique_font(TTF_OpenFont(path.c_str(), size));
        return std::make_pair(path.stem(), std::move(font));
    };
}
// determine if a font pointer is null from a name-pointer pair
bool _font_is_null(unique_font const & font) {
    return not font;
}

result<unique_font_table>
load_all_fonts(fs::path const & dir, uint resolution)
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

    unique_font_table table;
    auto into_table = std::inserter(table, table.end());
    views::transform(font_files, into_table, _load_as_pair_with_size(resolution));

    // abort if any os calls failed in the process
    // or if some fonts couldn't be loaded
    auto ttf_fonts = table | views::values;
    bool const loading_failed = ranges::any_of(ttf_fonts, &_font_is_null);
    if (ec or loading_failed) {
        std::string message;
        if (ec) { message = ec.message(); }
        if (loading_failed) { message = TTF_GetError(); }
        return tl::unexpected(message);
    }

    return table;
}

font_table observe_fonts(unique_font_table const & fonts)
{
    font_table table;
    for (auto &[name, font] : fonts) {
        table[name] = font.get();
    }
    return table;
}
}
