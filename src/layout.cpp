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