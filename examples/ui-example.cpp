// frameworks
#include "ion/system.hpp"
#include "ion/ui.hpp"
#include <SDL2/SDL.h>
#include <entt/entity/registry.hpp>

// library
#include "gold/layout.hpp"

// data types and structure
#include <string>
#include <vector>

// serialization and i/o
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

// type constraints and algorithms
#include <concepts>
#include <ranges>
#include <algorithm>
namespace ranges = std::ranges;
namespace views = std::views;
namespace align = gold::align;

namespace paths {
static fs::path const assets = fs::canonical("assets");
static fs::path const system_config = assets/"system.yaml";
static fs::path const widget_config = assets/"widget.yaml";
}
void print_error(std::string const & message) {
    std::cout << message << "\n\n";
}
namespace gold {

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
        auto const config = YAML::LoadFile(paths::widget_config.string());

        gold::layout layout;
        if (auto const layout_config = config["layout"]) {
            konbu::read(layout_config, layout, errors);
        }
        widgets.emplace<gold::background_color>(
            square, 0.429f, 0.160f, 0.480f, 0.540f);
        widgets.emplace<gold::size>(square, 100.f, 100.f);
        widgets.emplace<align::horizontal>(square, layout.horizontal);
        widgets.emplace<align::vertical>(square, layout.vertical);
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