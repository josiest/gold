#include "konbu/konbu.h"

template<std::ranges::output_range<YAML::Exception> error_output>
inline void konbu::read(YAML::Node const & config,
                        gold::size & size,
                        error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    if (config.IsScalar()) {
        konbu::read(config, size.width, errors);
        size.height = size.width;
    }
    else if (config.IsSequence() and config.size() == 2) {
        konbu::read(config[0], size.width, errors);
        konbu::read(config[1], size.height, errors);
    }
    else if (config.IsSequence()) {
        YAML::Exception const error{ config.Mark(),
                                     "expecting exactly two values" };
        ranges::copy(views::single(error),
                     konbu::back_inserter_preference(errors));
    }
    else if (config.IsMap()) {
        if (auto const width_config = config["width"]) {
            konbu::read(width_config, size.width, errors);
        }
        if (auto const height_config = config["height"]) {
            konbu::read(height_config, size.height, errors);
        }
    }
    else {
        YAML::Exception const error{ config.Mark(),
                                     "encountered an unknown error" };
        ranges::copy(views::single(error),
                     konbu::back_inserter_preference(errors));
    }
}