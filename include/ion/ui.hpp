#pragma once
#include <ranges>
#include <functional>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace ImGui {

namespace Defaults {
const ImVec2 ScrollboxSize{ 250.f, 70.f };
const ImVec2 ButtonSize{ 100.f, 20.f };
const ImVec2 WindowSize{ 400.f, 300.f };
}

template<std::ranges::range Range,
         std::invocable<std::ranges::range_value_t<Range>> Projection = std::identity>
requires std::convertible_to<
    std::invoke_result_t<Projection, std::ranges::range_value_t<Range>>,
    std::string>
void Scrollbox(Range const& elements,
               Projection const & project = {},
               ImVec2 const& scrollbox_size = Defaults::ScrollboxSize)
{
    ImGui::BeginChild("Scrollbox", scrollbox_size, true);
    for (auto const & elem : elements) {
        std::string const string_value = project(elem);
        ImGui::Text(string_value.c_str());
    }
    ImGui::EndChild();
}

template<std::ranges::range Range,
         std::invocable<std::ranges::range_value_t<Range>> ValueConsumer,
         std::invocable<std::ranges::range_value_t<Range>> Projection = std::identity>

requires std::convertible_to<
    std::invoke_result_t<Projection, std::ranges::range_value_t<Range>>,
    std::string>

void Scrollbox(Range const& elements,
               ValueConsumer const & on_selected,
               Projection const & project = {},
               ImVec2 const& scrollbox_size = Defaults::ScrollboxSize)
{
    ImGui::BeginChild("Scrollbox", scrollbox_size, true);
    for (auto const & elem : elements) {
        std::string const string_value = project(elem);
        if (ImGui::Selectable(string_value.c_str())) {
            on_selected(elem);
        }
    }
    ImGui::EndChild();
}

template<std::ranges::range Range,
         std::invocable<std::ranges::range_value_t<Range> const &> Callback,
         std::invocable<std::ranges::range_value_t<Range> const &> Projection
            = std::identity>

requires std::convertible_to<
    std::invoke_result_t<Projection, std::ranges::range_value_t<Range> const &>,
    std::string>

void ButtonList(Range const & elems,
                Callback const & on_click,
                Projection const & as_text = {},
                ImVec2 const & button_size = Defaults::ButtonSize)
{
    for (auto const & elem : elems) {
        if (Button(as_text(elem).c_str(), button_size)) {
            on_click(elem);
        }
    }
}

struct TextView {
    std::optional<ImVec4> color = std::nullopt;
};
struct ColumnView {
    std::string id = "##column";

    ImGuiTableColumnFlags flags = ImGuiTableColumnFlags_None;
    float init_width = 0.f;
};
void TableSetupColumn(ColumnView const & params);

struct WindowView {
    std::string id = "##window";

    bool * is_open = nullptr;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    ImVec2 position = {
        GetMainViewport()->WorkPos.x + 50,
        GetMainViewport()->WorkPos.y + 50,
    };
    ImVec2 size = Defaults::WindowSize;
};
bool NewWindow(WindowView const & params = {});

template<std::invocable Callback>
void Button(std::string const & text,
            Callback const & on_selected,
            ImVec2 size = ImGui::Defaults::ButtonSize)
{
    if (Button(text.c_str(), size)) { on_selected(); }
}
}
