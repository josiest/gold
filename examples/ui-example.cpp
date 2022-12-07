// frameworks
#include "ion/system.hpp"
#include "ion/ui.hpp"
#include <SDL2/SDL.h>
#include <entt/entity/registry.hpp>

// library
#include "gold/render.hpp"
#include "gold/layout.hpp"
#include "gold/size.hpp"
#include "gold/background_color.hpp"

// data types and structure
#include <string>
#include <vector>

// serialization and i/o
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

// type constraints and algorithms
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
namespace global {
bool show_demo = false;
bool show_editor = false;
}

namespace gold {
struct editor {
    entt::registry widgets;
    entt::entity selected_widget = entt::null;
};
}

namespace ImGui {
void ShowHorizontalAlignOptions(entt::registry & widgets, entt::entity widget)
{
    auto & selected_option = widgets
        .get_or_emplace<align::horizontal>(widget, align::horizontal::left);
    for (int i = 0; i < 4; ++i) {
        align::horizontal option{ i };
        if (ImGui::Selectable(gold::to_string(option).c_str(),
                              option == selected_option)) {
            selected_option = option;
        }
    }
}

void ShowEditorWindow(bool * is_open, gold::editor & editor)
{
    ImGui::WindowView window_params;
    window_params.id = "Widget Editor";
    window_params.is_open = is_open;
    window_params.position = { 500.f, 50.f };

    if (not ImGui::NewWindow(window_params)) {
        ImGui::End();
        return;
    }
    ShowHorizontalAlignOptions(editor.widgets, editor.selected_widget);
    ImGui::End();
}
}

entt::entity make_square(entt::registry & widgets)
{
    std::vector<YAML::Exception> errors;
    auto const config = YAML::LoadFile(paths::widget_config.string());

    gold::layout layout;
    if (auto const layout_config = config["layout"]) {
        konbu::read(layout_config, layout, errors);
    }
    auto const square = widgets.create();
    widgets.emplace<gold::background_color>(
        square, 0.429f, 0.160f, 0.480f, 0.540f);
    widgets.emplace<gold::size>(square, 100.f, 100.f);
    widgets.emplace<align::horizontal>(square, layout.horizontal);
    widgets.emplace<align::vertical>(square, layout.vertical);
    return square;
}

void render(SDL_Window *)
{
    static gold::editor editor;
    static bool has_init = false;
    if (not has_init) {
        editor.selected_widget = make_square(editor.widgets);
        has_init = true;
    }
    if (global::show_demo) {
        ImGui::ShowDemoWindow(&global::show_demo);
    }
    if (global::show_editor) {
        ImGui::ShowEditorWindow(&global::show_editor, editor);
    }
    if (not ImGui::NewWindow()) {
        ImGui::End();
        return;
    }
    // draw example widget centered
    editor.widgets.each([](auto widget) {
        gold::render(editor.widgets, widget);
    });
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

void toggle_editor(SDL_Keysym const & sym)
{
    auto constexpr editor_mask = KMOD_CTRL;
    bool const can_toggle = mask_contains<editor_mask>(sym.mod) and
                            has_mask<KMOD_CTRL>(sym.mod);

    if (sym.sym == SDLK_e and can_toggle) {
        global::show_editor = not global::show_editor;
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
    system->on_keydown().connect<&toggle_editor>();
    system->start();
    return EXIT_SUCCESS;
}