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

// algorithms
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
namespace just {
/** Horizontal justification setting */
enum class horizontal {
    left,   /** Widget should be left-justified */
    right,  /** Widget should be right-justified */
    center, /** Widget should be centered horizontally */
    fill    /** Widget should horizontally fill its layout */
};
/** Vertical justification setting */
enum class vertical {
    top,    /** Widget should be anchored to the top */
    bottom, /** Widget should be anchored to the bottom */
    center, /** Widget should be centered vertically */
    fill    /** Widget should vertically fill its layout */
};
}
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
        if (auto const * hjust =
                widgets.try_get<gold::just::horizontal>(widget)) {

            float const avail_width = ImGui::GetContentRegionAvail().x;
            float const cursor_x = ImGui::GetCursorPosX();
            float x_offset = 0.f;

            switch (*hjust) {
                case gold::just::horizontal::right:
                    x_offset = avail_width - size->width;
                    ImGui::SetCursorPosX(cursor_x + x_offset);
                    break;
                case gold::just::horizontal::center:
                    x_offset = (avail_width - size->width)/2.f;
                    ImGui::SetCursorPosX(cursor_x +  x_offset);
                    break;
                case gold::just::horizontal::fill:
                    desired_size.x = 0.f;
                    break;
                case gold::just::horizontal::left:
                default:
                    break;
            }
        }
        ImGui::BeginChild(id.c_str(), desired_size);
    }
    else {
        ImGui::BeginChild(id.c_str());
    }
    ImGui::EndChild();
    if (color) {
        ImGui::PopStyleColor();
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
        widgets.emplace<gold::background_color>(
            square, 0.429f, 0.160f, 0.480f, 0.540f);
        widgets.emplace<gold::size>(square, 100.f, 100.f);
        namespace just = gold::just;
        widgets.emplace<just::horizontal>(square, just::horizontal::center);
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