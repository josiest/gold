#include "konbu/konbu.h"
#include "gold/utility.hpp"
#include <yaml-cpp/yaml.h>

#include <ranges>
#include <unordered_map>
#include <unordered_set>

template<std::ranges::output_range<YAML::Exception> error_output>
inline void konbu::read(YAML::Node const & config,
                        gold::align::horizontal & halign,
                        error_output & errors)
{
    namespace align = gold::align;
    static std::unordered_map<std::string, align::horizontal> const
        as_halign {
        { "left",   align::horizontal::left },
        { "right",  align::horizontal::right },
        { "center", align::horizontal::center },
        { "fill",   align::horizontal::fill }
    };
    // read the errors first into an isolated list, so that we can
    // re-contextualize them before copying them into the main error list
    konbu::read_lookup(config, halign, as_halign, errors);
}

template <std::ranges::output_range<YAML::Exception> error_output>
inline void konbu::read(YAML::Node const & config,
                        gold::align::vertical & valign,
                        error_output & errors)
{
    namespace align = gold::align;
    static std::unordered_map<std::string, align::vertical> const
        as_valign {
        { "top",     align::vertical::top },
        { "bottom",  align::vertical::bottom },
        { "center",  align::vertical::center },
        { "fill",    align::vertical::fill }
    };
    // read the errors first into an isolated list, so that we can
    // re-contextualize them before copying them into the main error list
    konbu::read_lookup(config, valign, as_valign, errors);
}

template<std::ranges::output_range<YAML::Exception> error_output>
inline void konbu::read(YAML::Node const & config,
                        gold::layout & layout,
                        error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    YAML::Node horizontal_config;
    YAML::Node vertical_config;

    if (config.IsScalar()) {
        std::unordered_set<std::string> const valid_names{ "center", "fill" };
        if (valid_names.find(config.Scalar()) != valid_names.end()) {
            horizontal_config = config;
            vertical_config = config;
        }
        else {
            YAML::Exception const error{ config.Mark(),
                                         R"(expecting "center" or "fill")" };
            ranges::copy(views::single(error),
                         konbu::back_inserter_preference(errors));
            return;
        }
    }
    else if (config.IsSequence() and config.size() == 2) {
        horizontal_config = config[0];
        vertical_config = config[1];
    }
    else if (config.IsSequence()) {
        YAML::Exception const error{ config.Mark(),
                                     "expecting exactly two values" };
        ranges::copy(views::single(error),
                     konbu::back_inserter_preference(errors));
        return;
    }
    else if (config.IsMap()) {
        horizontal_config = config["horizontal"];
        vertical_config = config["vertical"];
    }
    if (horizontal_config) {
        std::vector<YAML::Exception> horizontal_errors;
        konbu::read(horizontal_config, layout.horizontal, horizontal_errors);
        ranges::transform(horizontal_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("horizontal",
                                                     layout.horizontal));
    }
    if (vertical_config) {
        std::vector<YAML::Exception> vertical_errors;
        konbu::read(vertical_config, layout.vertical, vertical_errors);
        ranges::transform(vertical_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("vertical",
                                                     layout.vertical));
    }
}