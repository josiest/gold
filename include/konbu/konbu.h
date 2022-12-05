#pragma once

// data types and resource handles
#include <optional>
#include <expected>

// type constraints
#include <concepts>
#include <ranges>

// i/o
#include <sstream>
#include <regex>
#include <yaml-cpp/yaml.h>

namespace konbu {

/**
 * \brief a container that can `push_back`
 * \tparam container an allocator-aware container
 */
template<typename container>
concept can_push_back =
requires(container & c, container::value_type const & v)
{
    c.push_back(v);
};

/**
 * \brief A container that can `push_front`
 * \tparam container an allocator-aware container
 */
template<typename container>
concept can_push_front =
requires(container & c, container::value_type const & v)
{
    c.push_front(v);
};

/**
 * \brief An insert iterator following the back_inserter_preference
 * \tparam container    an allocator-aware container
 * \param c container to insert into
 * \return an insert_iterator for the container, inserting from ranges::begin
 */
template<std::ranges::range container>
auto back_inserter_preference(container & c)
{
    return std::inserter(c, std::ranges::begin(c));
}

/**
 * \brief An insert iterator following the back_inserter_preference
 * \tparam container    an allocator-aware container that can `push_back`
 * \param c container to insert into
 * \return a back_insert_iterator for the container
 */
template<std::ranges::range container>
requires can_push_back<container>
auto back_inserter_preference(container & c)
{
    return std::back_inserter(c);
}

/**
 * \brief An insert iterator following the back_inserter_preference
 * \tparam container    an allocator-aware container that can `push_front`,
 *                      but not `push_back`
 *
 * \param c container to insert into
 *
 * \return a front_insert_iterator to the container
 */
template<std::ranges::range container>
requires can_push_front<container> and (not can_push_back<container>)
auto back_inserter_preference(container & c)
{
    return std::front_inserter(c);
}

/** The key type of map-container */
template<typename container>
using lookup_key_t = typename container::key_type;

/** The mapped type of a map-container */
template <typename container>
using lookup_mapped_t = typename container::mapped_type;

/**
 * \brief A map container type
 * \tparam container can find a key and return a pair iterator
 */
template<typename container>
concept lookup_table =
requires(container const & c, lookup_key_t<container> const & key)
{
    { c.find(key) } -> std::indirectly_readable;
    { c.find(key)->first } -> std::convertible_to<lookup_key_t<container>>;
    { c.find(key)->second } -> std::convertible_to<lookup_mapped_t<container>>;
};

/**
 * \brief parse an arbitrary type from a name-lookup
 *
 * \tparam name_lookup      maps strings to value types
 * \tparam error_output     allocator-aware container of yaml-exceptions
 *
 * \param config    YAML string input
 * \param value     write parsed value to
 * \param lookup    maps names to their desired values
 * \param errors    write any parsing errors to
 */
template<lookup_table name_lookup,
         std::ranges::output_range<YAML::Exception> error_output>
requires std::convertible_to<std::string, lookup_key_t<name_lookup>>

void read_lookup(YAML::Node const & config,
                 lookup_mapped_t<name_lookup> & value,
                 name_lookup const & lookup,
                 error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;
    if (not config.IsScalar()) {
        YAML::Exception const error{ config.Mark(), "expecting a string" };
        ranges::copy(views::single(error),
                     back_inserter_preference(errors));
        return;
    }
    auto const search = lookup.find(config.as<std::string>());
    if (search != lookup.end()) {
        value = search->second;
        return;
    }
    std::stringstream message;
    message << "expecting value to be one of the following: [";
    std::string sep;
    for (const auto & name : lookup | views::keys) {
        message << sep << name;
        sep = ", ";
    }
    message << "]";
    YAML::Exception const error{ config.Mark(), message.str() };
    ranges::copy(views::single(error),
                 back_inserter_preference(errors));
}

/**
 * \brief Read a string value from config
 *
 * \tparam string_like      can be converted to a string
 * \tparam error_output     an allocator-aware container of yaml-exceptions
 *
 * \param config    YAML string input
 * \param value     write the parsed string to
 * \param errors    write any parsing errors to
 */
template<typename string_like,
         std::ranges::output_range<YAML::Exception> error_output>
requires std::convertible_to<std::string, string_like>
void read(YAML::Node const & config, string_like & value, error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    if (not config.IsScalar()) {
        YAML::Exception const error{ config.Mark(), "expecting a string" };
        ranges::copy(views::single(error),
                     back_inserter_preference(errors));
        return;
    }
    value = config.Scalar();
}

/**
 * \brief Read an integer point number from config
 *
 * \tparam number           integer type
 * \tparam error_output     allocator-aware range of yaml-exceptions
 *
 * \param config    YAML integer input
 * \param value     write parsed integer to
 * \param errors    write any parsing errors to
 *
 * \note Reading a negative number from `config` for an unsigned `number` type
 *       will result in an error written to `errors`.
 */
template<std::integral number,
         std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config, number & value, error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    if (not config.IsScalar()) {
        YAML::Exception const error{ config.Mark(), "expecting an integer" };
        ranges::copy(views::single(error),
                     back_inserter_preference(errors));
        return;
    }
    std::regex const negative_pattern{ "^-" };
    if (std::is_unsigned_v<number> and
        std::regex_search(config.Scalar(), negative_pattern)) {

        YAML::Exception const error{ config.Mark(),
                                     "expecting a non-negative integer" };
        ranges::copy(views::single(error),
                     back_inserter_preference(errors));
        return;
    }
    std::regex const integer_pattern{ "-?[0-9]+[ \t]*" };
    if (not std::regex_match(config.Scalar(), integer_pattern)) {
        YAML::Exception const error{ config.Mark(), "expecting an integer" };
        ranges::copy(views::single(error),
                     back_inserter_preference(errors));
        return;
    }
    value = config.as<number>();
}

/**
 * \brief Read a floating point number from config
 *
 * \tparam number           floating-point type
 * \tparam error_output     allocator aware container of yaml-exceptions
 *
 * \param config    YAML floating point input
 * \param value     write parsed number to
 * \param errors    write any parsing errors to
 */
template<std::floating_point number,
         std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config, number & value, error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;
    if (not config.IsScalar()) {
        YAML::Exception const error{ config.Mark(), "expecting a number" };
        ranges::copy(views::single(error),
                     back_inserter_preference(errors));
        return;
    }
    std::regex const integer_pattern{ "-?[0-9]+\\.?" };
    std::regex const decimal_pattern{ "-?\\.[0-9]+" };
    std::regex const real_pattern{ "-?[0-9]+\\.[0-9]+" };

    std::string const& scalar_value = config.Scalar();
    if (not std::regex_match(scalar_value, integer_pattern) and
        not std::regex_match(scalar_value, decimal_pattern) and
        not std::regex_match(scalar_value, real_pattern)) {

        YAML::Exception const error{ config.Mark(), "expecting a number" };
        ranges::copy(views::single(error),
                     back_inserter_preference(errors));
        return;
    }
    value = config.as<number>();
}

/**
 * \brief Models a type that can be read by the konbu read interface
 * \tparam value the value-type to read
 */
template<typename value>
concept readable =
requires(YAML::Node const & node, value & v, std::vector<YAML::Exception> & errors)
{
    konbu::read(node, v, errors);
};

/**
 * \brief Parse a sequence of values
 *
 * \tparam value_output     allocator-aware container of konbu-readable types
 * \tparam error_output     allocator-aware container of yaml-exceptions
 *
 * \param sequence  YAML sequence input of desired values
 * \param values    write parsed values to
 * \param errors    write any parsing errors to
 *
 * All valid config values in the sequence will be parsed into `values`. Any
 * config values that fail to parse won't be written to `values`, and the error
 * will be written to `errors`
 */
template<std::ranges::range value_output,
         std::ranges::output_range<YAML::Exception> error_output>
requires readable<std::ranges::range_value_t<value_output>>

void partition_expect(YAML::Node const & sequence,
                      value_output & values,
                      error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;
    using value_t = ranges::range_value_t<value_output>;

    if (not sequence.IsSequence()) {
        YAML::Exception const error{ sequence.Mark(), "expecting a sequence" };
        ranges::copy(views::single(error), back_inserter_preference(errors));
        return;
    }
    std::vector<YAML::Exception> sequence_errors;
    for (YAML::Node const & node : sequence) {
        value_t value;
        auto const num_errors = sequence_errors.size();
        konbu::read(node, value, sequence_errors);

        if (sequence_errors.size() != num_errors) {
            continue;
        }
        ranges::copy(views::single(value),
                     back_inserter_preference(values));
    }
    auto contextualize = [](YAML::Exception const & error) {
        std::stringstream message;
        message << "couldn't read sequence value: " << error.msg;
        return YAML::Exception{ error.mark, message.str() };
    };
    ranges::copy(sequence_errors | views::transform(contextualize),
                 back_inserter_preference(errors));
}

/**
 * \brief Read flag values from a config node.
 *
 * \tparam flag_lookup          maps strings to flag-types
 * \tparam error_output         allocator-aware container of yaml-exceptions
 *
 * \param flagname_sequence     YAML input sequence of desired values
 * \param flags                 write parsed flags to
 * \param lookup                mapping of flag names to their int-values
 * \param errors                write any parsing errors to
 *
 * All valid flagnames from the `flagname_sequence` are parsed into a
 * non-negative integer define by the mapping `lookup`, and unioned into flags.
 * If no valid flags were parsed, the value existing in flags will be used.
 * Any invalid flagnames or other parsing errors will be written to `errors`
 */
template<lookup_table flag_lookup,
         std::ranges::output_range<YAML::Exception> error_output>
requires std::convertible_to<std::string, lookup_key_t<flag_lookup>> and
         std::unsigned_integral<lookup_mapped_t<flag_lookup>>

void read_flags(YAML::Node const & flagname_sequence,
                lookup_mapped_t<flag_lookup> & flags,
                flag_lookup const & lookup,
                error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    if (not flagname_sequence.IsSequence()) {
        YAML::Exception const error{ flagname_sequence.Mark(),
                                     "expecting a sequence" };
        ranges::copy(views::single(error), back_inserter_preference(errors));
        return;
    }
    lookup_mapped_t<flag_lookup> parsed_flags = 0u;
    auto parse_valid = [&lookup, &parsed_flags](std::string const & name) {
        auto const search = lookup.find(name);
        if (search != lookup.end()) {
            parsed_flags |= search->second;
            return true;
        }
        return false;
    };
    // partition algorithm
    std::vector<YAML::Exception> flagname_errors;
    for (YAML::Node const & node : flagname_sequence) {

        std::string name;
        auto const num_errors = flagname_errors.size();
        konbu::read(node, name, flagname_errors);

        if (flagname_errors.size() != num_errors) {
            continue;
        }
        if (parse_valid(name)) {
            continue;
        }
        std::stringstream message;
        message << "no flag named \"" << name << "\"\n  "
                << "expecting name to be one of the following: [";
        std::string sep;
        for (const auto & flag : lookup | views::keys) {
            message << sep << flag;
            sep = ", ";
        }
        message << "]";
        YAML::Exception const error{ node.Mark(), message.str() };
        ranges::copy(views::single(error),
                     back_inserter_preference(flagname_errors));
    }
    if (parsed_flags != 0u) {
        flags = parsed_flags;
    }
    auto contextualize = [](YAML::Exception const & error) {
        std::stringstream message;
        message << "couldn't parse flag: " << error.msg;
        return YAML::Exception{ error.mark, message.str() };
    };
    ranges::copy(flagname_errors | views::transform(contextualize),
                 back_inserter_preference(errors));
}

template<typename value>
concept string_streamable =
requires(std::stringstream & stream, value const & v) {
    stream << v;
};

/**
 * \brief Re-contextualize a yaml-exception to include parameter info
 *
 * \tparam value        can be output to a string stream
 *
 * \param param_name    the name of the yaml parameter
 * \param default_value the default value being used
 *
 * \return a monadic function that adds a parameter context to a yaml exception
 */
template<string_streamable value>
auto contextualize_param(std::string const & param_name,
                         value const & default_value)
{
    return [&param_name, &default_value](YAML::Exception const & error) {
        std::stringstream message;
        message << "couldn't parse \"" << param_name << "\" parameter: "
                << error.msg << "\n  using default value of " << default_value;
        return YAML::Exception{ error.mark, message.str() };
    };
}

/**
 * \brief Re-contextualize a yaml error to include setting-name info
 * \param setting_name  the name of the setting being parsed
 * \return a monadic function that adds a setting context to a yaml exception
 */
inline auto contextualize_setting(std::string const & setting_name)
{
    return [&setting_name](YAML::Exception const & error) {
        std::stringstream message;
        message << "encountered error reading " << setting_name
                << " setting\n  " << error.msg;
        return YAML::Exception{ error.mark, message.str() };
    };
}

/**
 * \brief read a simple version string
 *
 * \tparam number           non-negative integer
 * \tparam error_output     allocator-aware container of yaml-exceptions
 *
 * \param input             yaml input for version string
 * \param major_version     write major version to
 * \param minor_version     write minor version to
 * \param errors            write any parsing errors to
 */
template<std::unsigned_integral number,
    std::ranges::output_range<YAML::Exception> error_output>
void read_version(YAML::Node const & input,
                  number & major_version, number & minor_version,
                  error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;
    if (not input.IsScalar()) {
        YAML::Exception const error{ input.Mark(),
                                     "expecting a version string" };
        ranges::copy(views::single(error),
                     back_inserter_preference(errors));
        return;
    }
    YAML::Exception const format_error{
        input.Mark(),
        "version string must have the form \"<major>.<minor>\""
    };
    std::regex const version_pattern{ "([0-9]+)\\.([0-9]+)" };
    std::smatch version_match;
    if (not std::regex_search(input.Scalar(), version_match, version_pattern)) {
        ranges::copy(views::single(format_error),
                     back_inserter_preference(errors));
        return;
    }
    if (version_match.size() != 3) {
        ranges::copy(views::single(format_error),
                     back_inserter_preference(errors));
        return;
    }
    YAML::Node const major_config{ version_match[1].str() };
    major_version = major_config.as<number>();

    YAML::Node const minor_config{ version_match[2].str() };
    minor_version = minor_config.as<number>();
}
}