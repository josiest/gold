// frameworks
#include "ion/system.hpp"
#include "ion/ui.hpp"
#include <SDL2/SDL.h>
#include <entt/entity/registry.hpp>

// data types and structure
#include <string>
#include <vector>

// serialization and i/o
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <iostream>

// type constraints and algorithms
#include <concepts>
#include <ranges>
#include <algorithm>
namespace ranges = std::ranges;
namespace views = std::views;

namespace paths {
static std::filesystem::path const assets =
    std::filesystem::canonical("../assets");

static std::filesystem::path const system_config = assets/"system.yaml";
}
void print_error(std::string const & message) {
    std::cout << message << "\n\n";
}

namespace gold {
namespace align {
/** Horizontal alignment setting */
enum class horizontal {
    left,   /** Widget should be left-aligned */
    right,  /** Widget should be right-aligned */
    center, /** Widget should be centered horizontally */
    fill    /** Widget should horizontally fill its layout */
};
/** Vertical alignment setting */
enum class vertical {
    top,    /** Widget should be anchored to the top */
    bottom, /** Widget should be anchored to the bottom */
    center, /** Widget should be centered vertically */
    fill    /** Widget should vertically fill its layout */
};
}

std::string to_string(align::horizontal const & horz) {
    using namemap = std::unordered_map<align::horizontal, std::string>;
    static namemap const names{
        { align::horizontal::left,   "left" },
        { align::horizontal::right,  "right" },
        { align::horizontal::center, "center" },
        { align::horizontal::fill,   "fill" }
    };
    return names.find(horz)->second;
}

std::string to_string(align::vertical const & vert) {
    using namemap = std::unordered_map<align::vertical, std::string>;
    static namemap const names{
        { align::vertical::top,    "top" },
        { align::vertical::bottom, "bottom" },
        { align::vertical::center, "center" },
        { align::vertical::fill,   "fill" }
    };
    return names.find(vert)->second;
}

template<typename value>
concept string_convertible =
requires(value const & v) {
    { gold::to_string(v) } -> std::convertible_to<std::string>;
};
}
namespace align = gold::align;

namespace std {
template<gold::string_convertible value>
std::ostream & operator<<(std::ostream & os, value const & v) {
    return os << gold::to_string(v);
}
}

namespace gold {
/** Define how a widget will be aligned in the layout */
struct layout {
    align::horizontal horizontal = align::horizontal::left;
    align::vertical vertical = align::vertical::top;
};
/** A widget will render with the desired size. */
struct size {
    float width = 0.f;
    float height = 0.f;

    [[nodiscard]] constexpr ImVec2 vector() const;
};

/** A widget will render with the desired color. */
struct background_color {
    float red = 0.f;
    float green = 0.f;
    float blue = 0.f;
    float alpha = 1.f;

    [[nodiscard]] constexpr ImVec4 vector() const;
};

void render(entt::registry & widgets, entt::entity widget);
}
constexpr ImVec2 gold::size::vector() const
{
    return ImVec2{ width, height };
}
constexpr ImVec4 gold::background_color::vector() const
{
    return ImVec4{ red, green, blue, alpha };
}
void gold::render(entt::registry & widgets, entt::entity widget)
{
    auto const * color = widgets.try_get<gold::background_color>(widget);
    if (color) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, color->vector());
    }
    auto const id = std::to_string(static_cast<std::uint32_t>(widget));
    if (auto const * size = widgets.try_get<gold::size>(widget)) {
        auto desired_size = size->vector();
        if (auto const * halign = widgets.try_get<align::horizontal>(widget)) {

            float const avail_width = ImGui::GetContentRegionAvail().x;
            float const cursor_x = ImGui::GetCursorPosX();
            float x_offset;

            switch (*halign) {
                case align::horizontal::right:
                    x_offset = avail_width - size->width;
                    ImGui::SetCursorPosX(cursor_x + x_offset);
                    break;
                case align::horizontal::center:
                    x_offset = (avail_width - size->width)/2.f;
                    ImGui::SetCursorPosX(cursor_x +  x_offset);
                    break;
                case align::horizontal::fill:
                    desired_size.x = 0.f;
                    break;
                case align::horizontal::left:
                default:
                    break;
            }
        }
        if (auto const * valign = widgets.try_get<align::vertical>(widget)) {
            float const avail_height = ImGui::GetContentRegionAvail().y;
            float const cursor_y = ImGui::GetCursorPosY();
            float y_offset;

            switch (*valign) {
            case align::vertical::bottom:
                y_offset = avail_height - size->height;
                // TODO: be careful with the cursor!! (maybe use a push fn instead?)
                ImGui::SetCursorPosY(cursor_y + y_offset);
                break;
            case align::vertical::center:
                y_offset = (avail_height - size->height)/2.f;
                // TODO: be careful with the cursor!! (maybe use a push fn instead?)
                ImGui::SetCursorPosY(cursor_y + y_offset);
                break;
            case align::vertical::fill:
                desired_size.y = 0.f;
                break;
            case align::vertical::top:
            default:
                break;
            }
        }
        ImGui::BeginChild(id.c_str(), desired_size);
    }
    else {
        ImGui::BeginChild(id.c_str());
    }
    if (color) {
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();
}

namespace konbu {
template<ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          align::horizontal & halign,
          error_output & errors)
{
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

template <ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          align::vertical & valign,
          error_output & errors)
{
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

template<ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
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
}

namespace global {
bool show_demo = false;
}

void render(SDL_Window *)
{
    if (global::show_demo) {
        ImGui::ShowDemoWindow(&global::show_demo);
    }
    if (not ImGui::NewWindow()) {
        ImGui::End();
        return;
    }
    static entt::registry widgets;
    static auto const square = widgets.create();
    static bool has_init = false;
    if (not has_init) {
        std::vector<YAML::Exception> errors;

        YAML::Node const halign_config{"center"};
        YAML::Node const valign_config{"fill"};

        auto halign = align::horizontal::left;
        konbu::read(halign_config, halign, errors);

        auto valign = align::vertical::top;
        konbu::read(valign_config, valign, errors);

        widgets.emplace<gold::background_color>(
            square, 0.429f, 0.160f, 0.480f, 0.540f);
        widgets.emplace<gold::size>(square, 100.f, 100.f);
        widgets.emplace<align::horizontal>(square, halign);
        widgets.emplace<align::vertical>(square, valign);
        has_init = true;
    }

    // draw example widget centered
    widgets.each([](auto widget) { gold::render(widgets, widget); });

    // center the widget
    // float const avail_width = ImGui::GetContentRegionAvail().x;
    // float const x_offset = (avail_width - size.x)/2.f;
    // ImGui::SetCursorPosX(ImGui::GetCursorPosX() +  x_offset);

    ImGui::End();
}

template<std::uint16_t mask>
inline constexpr bool mask_contains(std::uint16_t mod)
{
    std::uint16_t constexpr all = KMOD_SHIFT | KMOD_ALT | KMOD_CTRL;
    mod &= all;
    return (mod & mask) == mod;
}

template<std::uint16_t mask>
inline constexpr bool has_mask(std::uint16_t mod)
{
    return (mod & mask) != KMOD_NONE;
}

void toggle_demo(SDL_Keysym const & sym)
{
    auto constexpr demo_mask = KMOD_CTRL;
    bool const can_toggle = mask_contains<demo_mask>(sym.mod) and
                            has_mask<KMOD_CTRL>(sym.mod);

    if (sym.sym == SDLK_d and can_toggle) {
        global::show_demo = not global::show_demo;
    }
}

int main()
{
    std::vector<YAML::Exception> yaml_errors;
    auto system = ion::system::from_config(paths::system_config, yaml_errors);

    ranges::for_each(yaml_errors | views::transform(&YAML::Exception::what),
                     print_error);
    if (not system) {
        print_error(system.error());
        return EXIT_FAILURE;
    }

    system->on_render().connect<&render>();
    system->on_keydown().connect<&toggle_demo>();
    system->start();
    return EXIT_SUCCESS;
}