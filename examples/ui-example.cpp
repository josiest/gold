// frameworks
#include "ion/system.hpp"
#include "ion/ui.hpp"
#include <SDL2/SDL.h>
#include <entt/entity/registry.hpp>

// library
#include "gold/component.hpp"
#include "gold/render.hpp"
#include "gold/layout.hpp"
#include "gold/size.hpp"
#include "gold/background_color.hpp"
#include "gold/widget.hpp"

// data types and structure
#include <string>
#include <vector>
#include <variant>

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

inline namespace gold {
struct editor {
    entt::registry widgets;
    entt::entity selected_widget = entt::null;
};
}

inline namespace gold {
template<class align_enum>
requires std::same_as<align_enum, gold::align::horizontal> or
         std::same_as<align_enum, gold::align::vertical>
void show_options(align_enum & selected_option) {
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

void show_options(gold::layout & layout) {
    if (not ImGui::BeginTable("Widget Components", 2)) {
        ImGui::EndTable();
        return;
    }
    ImGui::TableNextColumn();
    ImGui::Text("Horizontal");

    ImGui::TableNextColumn();
    gold::show_options(layout.horizontal);

    ImGui::TableNextColumn();
    ImGui::Text("Vertical");

    ImGui::TableNextColumn();
    gold::show_options(layout.vertical);
    ImGui::EndTable();
}

void show_options(gold::size & size) {
    if (not ImGui::BeginTable("Widget Size Input", 2)) {
        ImGui::EndTable();
        return;
    }
    ImGui::TableNextColumn();
    ImGui::Text("Width");
    ImGui::TableNextColumn();
    ImGui::Text("Height");

    ImGui::TableNextColumn();
    ImGui::DragFloat("##Widget-Width", &size.width, 1.f, 0.f, FLT_MAX, "%.1f");
    ImGui::TableNextColumn();
    ImGui::DragFloat("##Widget-Height", &size.height, 1.f, 0.f, FLT_MAX, "%.1f");
    ImGui::EndTable();
}

void show_options(gold::background_color & color) {
    float color_values[]{
        color.red, color.green, color.blue, color.alpha
    };
    ImGui::ColorEdit4("##Widget-Color", color_values);
    color.red = color_values[0];
    color.green = color_values[1];
    color.blue = color_values[2];
    color.alpha = color_values[3];
}
}
namespace ImGui {

template<gold::editor_option component>
requires gold::has_public_name<component>
std::optional<component>
ComboOption(entt::registry & widgets, entt::entity widget,
            std::string_view & selected_component)
{
    std::string_view constexpr name = component_info<component>::public_name;
    if (widgets.any_of<component>(widget)) {
        return std::nullopt;
    }
    if (ImGui::Selectable(name.data(), name == selected_component)) {
        selected_component = name;
        return component{};
    }
    return std::nullopt;
}

void ShowEditorWindow(bool * is_open, gold::editor & editor)
{
    ImGui::WindowView window_params;
    window_params.id = "Widget Editor";
    window_params.is_open = is_open;
    window_params.position = { 500.f, 50.f };
    window_params.size.y += 50.f;

    if (not ImGui::NewWindow(window_params)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Save Widget");
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
    ImGui::Separator();

    gold::show_component_options<gold::layout>(
        editor.widgets, editor.selected_widget);
    gold::show_component_options<gold::size>(
        editor.widgets, editor.selected_widget);
    gold::show_component_options<gold::background_color>(
        editor.widgets, editor.selected_widget);

    ImGui::Text("Add Component");
    ImGui::Spacing();
    ImGui::Indent();
    static std::variant<std::monostate, gold::layout,
                        gold::size, gold::background_color>
    new_component;
    static std::string_view selected_component;
    if (ImGui::BeginCombo("##Select Component", selected_component.data())) {
        if (auto const layout = ComboOption<gold::layout>(
            editor.widgets, editor.selected_widget, selected_component))
        {
            new_component = *layout;
        }
        else if (auto const size = ComboOption<gold::size>(
            editor.widgets, editor.selected_widget, selected_component))
        {
            new_component = *size;
        }
        else if (auto const bg_color = ComboOption<gold::background_color>(
            editor.widgets, editor.selected_widget, selected_component))
        {
            new_component = *bg_color;
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    auto add_component = [&editor](auto const & component) {
        editor.widgets.emplace<std::remove_cvref_t<decltype(component)>>(
            editor.selected_widget, component);

        new_component = std::monostate{};
        selected_component = "";
    };
    if (not std::holds_alternative<std::monostate>(new_component) and
            ImGui::Button("Add##add-component")) {
        std::visit(add_component, new_component);
    }
    ImGui::Unindent();
    ImGui::End();
}
}

void render_demo(SDL_Window *)
{
    static gold::editor editor;
    static bool has_init = false;
    if (not has_init) {
        std::vector<YAML::Exception> errors;
        auto const config = YAML::LoadFile(paths::widget_config.string());
        editor.selected_widget = konbu::read_widget(
            config, editor.widgets, errors);
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

    system->on_render().connect<&render_demo>();
    system->on_keydown().connect<&toggle_demo>();
    system->on_keydown().connect<&toggle_editor>();
    system->start();
    return EXIT_SUCCESS;
}