#include "gold/layout.hpp"
#include <unordered_map>
#include <string>

namespace align = gold::align;
std::string gold::to_string(align::horizontal const & horz) {
    using namemap = std::unordered_map<align::horizontal, std::string>;
    static namemap const names{
        { align::horizontal::left,   "left" },
        { align::horizontal::right,  "right" },
        { align::horizontal::center, "center" },
        { align::horizontal::fill,   "fill" }
    };
    return names.find(horz)->second;
}
std::string gold::to_string(align::vertical const & vert) {
    using namemap = std::unordered_map<align::vertical, std::string>;
    static namemap const names{
        { align::vertical::top,    "top" },
        { align::vertical::bottom, "bottom" },
        { align::vertical::center, "center" },
        { align::vertical::fill,   "fill" }
    };
    return names.find(vert)->second;
}
namespace align = gold::align;
YAML::Node YAML::convert<align::horizontal>::encode(align::horizontal halign)
{
    return YAML::Node{ gold::to_string(halign) };
}
YAML::Node YAML::convert<align::vertical>::encode(align::vertical valign)
{
    return YAML::Node{ gold::to_string(valign) };
}
YAML::Node YAML::convert<gold::layout>::encode(gold::layout layout)
{
    YAML::Node node;
    node.push_back(layout.horizontal);
    node.push_back(layout.vertical);
    return node;
}