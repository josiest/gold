#include "gold/layout.hpp"
#include "gold/size.hpp"
#include "gold/background_color.hpp"

template<std::ranges::output_range<YAML::Exception> error_output>
inline entt::entity
konbu::read_widget(YAML::Node const & config,
                   entt::registry & widgets,
                   error_output & errors)
{
    auto const widget = widgets.create();
    if (auto const layout_config = config["layout"]) {
        gold::layout layout;
        konbu::read(layout_config, layout, errors);
        widgets.emplace<gold::layout>(widget, layout);
    }
    else if (auto const alignment_config = config["alignment"]) {
        gold::layout layout;
        konbu::read(alignment_config, layout, errors);
        widgets.emplace<gold::layout>(widget, layout);
    }
    else if (auto const align_config = config["align"]) {
        gold::layout layout;
        konbu::read(align_config, layout, errors);
        widgets.emplace<gold::layout>(widget, layout);
    }
    if (auto const size_config = config["size"]) {
        gold::size size;
        konbu::read(size_config, size, errors);
        widgets.emplace<gold::size>(widget, size);
    }
    if (auto const color_config = config["bg-color"]) {
        gold::background_color bg_color;
        konbu::read(color_config, bg_color, errors);
        widgets.emplace<gold::background_color>(widget, bg_color);
    }
    return widget;
}
