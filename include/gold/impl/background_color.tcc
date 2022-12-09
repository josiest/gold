#include "konbu/konbu.h"

constexpr ImVec4 gold::background_color::vector() const
{
    return ImVec4{ red, green, blue, alpha };
}
constexpr float
gold::sq_dist(gold::background_color const & lhs,
              gold::background_color const & rhs)
{
    float const dr = lhs.red - rhs.red;
    float const dg = lhs.green - rhs.green;
    float const db = lhs.blue - rhs.blue;
    float const da = lhs.alpha - rhs.alpha;
    return (dr*dr) + (dg*dg) + (db*db) + (da*da);
}
template<std::ranges::output_range<YAML::Exception> error_output>
inline void
konbu::read(YAML::Node const & config,
            gold::background_color & color,
            error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;
    if (config.IsScalar()) {
        konbu::read(config, color.red, errors);
        color.blue = color.red;
        color.green = color.red;
    }
    else if (config.IsSequence() and config.size() >= 3
                                 and config.size() <= 4) {

        konbu::read(config[0], color.red, errors);
        konbu::read(config[1], color.green, errors);
        konbu::read(config[2], color.blue, errors);
        if (config.size() == 4) {
            konbu::read(config[3], color.alpha, errors);
        }
    }
    else if (config.IsSequence()) {
        YAML::Exception const error{ config.Mark(),
                                     "expecting exactly 3 or 4 values" };
        ranges::copy(views::single(error),
                     konbu::back_inserter_preference(errors));
    }
    else if (config.IsMap()) {
        if (auto const red_config = config["red"]) {
            konbu::read(red_config, color.red, errors);
        }
        else if (auto const r_config = config["r"]) {
            konbu::read(r_config, color.red, errors);
        }
        if (auto const green_config = config["green"]) {
            konbu::read(green_config, color.green, errors);
        }
        else if (auto const g_config = config["g"]) {
            konbu::read(g_config, color.green, errors);
        }
        if (auto const blue_config = config["blue"]) {
            konbu::read(blue_config, color.blue, errors);
        }
        else if (auto const b_config = config["b"]) {
            konbu::read(b_config, color.blue, errors);
        }
        if (auto const alpha_config = config["alpha"]) {
            konbu::read(alpha_config, color.alpha, errors);
        }
        else if (auto const a_config = config["a"]) {
            konbu::read(a_config, color.alpha, errors);
        }
    }
    else {
        YAML::Exception const error{ config.Mark(),
                                     "encountered an unknown error" };
        ranges::copy(views::single(error),
                     konbu::back_inserter_preference(errors));
    }
}

inline YAML::Node
YAML::convert<gold::background_color>::encode(
    gold::background_color const & color)
{
    float const drg = std::abs(color.red - color.green);
    float const drb = std::abs(color.red - color.blue);
    float const dgb = std::abs(color.green - color.blue);
    float const default_alpha = gold::background_color{}.alpha;
    float const err_alpha = std::abs(color.alpha - default_alpha);
    float constexpr eps = 0.1f;

    if (err_alpha >= eps or drg >= eps or drb >= eps or dgb >= eps) {
        YAML::Node node;
        node.push_back(color.red);
        node.push_back(color.green);
        node.push_back(color.blue);
        if (err_alpha >= eps) {
            node.push_back(color.alpha);
        }
        return node;
    }
    else {
        return YAML::Node{ color.red };
    }
}