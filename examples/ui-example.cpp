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
#include "gold/widget.hpp"

// data types and structure
#include <string>
#include <vector>

// serialization and i/o
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
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
template<class align_enum>
requires std::same_as<align_enum, gold::align::horizontal> or
         std::same_as<align_enum, gold::align::vertical>
void ShowAlignOptions(align_enum & selected_option)
{
    if (not ImGui::BeginTable("Alignment Options", 4)) {
        ImGui::EndTable();
        return;
    }
    for (int i = 0; i < 4; ++i) {
        ImGui::TableNextColumn();
        align_enum option{ i };
        if (ImGui::Selectable(gold::to_string(option).c_str(),
                              option == selected_option)) {
            selected_option = option;
        }
    }
    ImGui::EndTable();
}

void ShowLayoutOptions(entt::registry & widgets, entt::entity widget)
{
    auto * layout = widgets.try_get<gold::layout>(widget);
    if (not layout) {
        return;
    }
    if (not ImGui::BeginTable("Widget Components", 2)) {
        ImGui::EndTable();
        return;
    }
    ImGui::TableNextColumn();
    ImGui::Text("Horizontal");

    ImGui::TableNextColumn();
    ShowAlignOptions(layout->horizontal);

    ImGui::TableNextColumn();
    ImGui::Text("Vertical");

    ImGui::TableNextColumn();
    ShowAlignOptions(layout->vertical);
    ImGui::EndTable();
}
void ShowSizeOptions(entt::registry & widgets, entt::entity widget)
{
    auto * size = widgets.try_get<gold::size>(widget);
    if (not size) {
        return;
    }
    if (not ImGui::BeginTable("Widget Size Input", 2)) {
        ImGui::EndTable();
        return;
    }
    ImGui::TableNextColumn();
    ImGui::Text("Width");
    ImGui::TableNextColumn();
    ImGui::Text("Height");

    ImGui::TableNextColumn();
    ImGui::DragFloat("##Widget-Width", &size->width);
    ImGui::TableNextColumn();
    ImGui::DragFloat("##Widget-Height", &size->height);
    ImGui::EndTable();
}
void ShowColorOptions(entt::registry & widgets, entt::entity widget)
{
    auto * color = widgets.try_get<gold::background_color>(widget);
    if (not color) {
        return;
    }
    float color_values[] {
        color->red, color->green, color->blue, color->alpha
    };
    ImGui::ColorEdit4("##Widget-Color", color_values);
    color->red = color_values[0];
    color->green = color_values[1];
    color->blue = color_values[2];
    color->alpha = color_values[3];
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
    ImGui::Text("Alignment");
    ImGui::Spacing();

    ImGui::Indent();
    ImGui::ShowLayoutOptions(editor.widgets, editor.selected_widget);
    ImGui::Unindent();

    ImGui::Text("Size");
    ImGui::Spacing();

    ImGui::Indent();
    ImGui::ShowSizeOptions(editor.widgets, editor.selected_widget);
    ImGui::Unindent();

    ImGui::Text("Color");
    ImGui::Spacing();

    ImGui::Indent();
    ImGui::ShowColorOptions(editor.widgets, editor.selected_widget);
    ImGui::Unindent();

    ImGui::Spacing();
    static char widget_filename[128] = "widget.yaml";
    ImGui::InputText("##Widget-Path-Input", widget_filename,
                                            IM_ARRAYSIZE(widget_filename));

    auto const widget_path = paths::assets/widget_filename;
    ImGui::SameLine();
    static std::string saved_to;
    if (ImGui::Button("Save")) {
        YAML::Emitter out;
        gold::write(out, editor.widgets, editor.selected_widget);
        std::ofstream file{ widget_path.string(), std::ios_base::trunc };
        file << out.c_str();
        file.close();
        saved_to = widget_path.string();
    }
    if (not saved_to.empty()) {
        ImGui::Text("Widget saved to:");
        ImGui::Indent();
        ImGui::Text("%s", saved_to.c_str());
        ImGui::Unindent();
    }
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
        std::cout << "read layout config: ["
                  << gold::to_string(layout.horizontal) << ", "
                  << gold::to_string(layout.vertical) << "]\n";
    }
    auto const square = widgets.create();
    widgets.emplace<gold::background_color>(square);
    widgets.emplace<gold::size>(square, 100.f, 100.f);
    widgets.emplace<gold::layout>(square, layout);
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