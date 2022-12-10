#include "imgui/imgui.h"

template<gold::editor_option component_type>
requires gold::has_public_name<component_type>
inline void
gold::show_component_options(entt::registry & widgets, entt::entity widget)
{
    auto * component = widgets.try_get<component_type>(widget);
    if (not component) {
        return;
    }
    std::string_view constexpr name =
        component_info<component_type>::public_name;
    ImGui::Text("%s |", name.data());

    ImGui::SameLine();

    using namespace std::string_literals;
    std::string const id = "Remove##remove-component#"s + std::string{name};
    if (ImGui::Button(id.c_str())) {
        widgets.erase<component_type>(widget);
        return;
    }
    ImGui::Spacing();

    ImGui::Indent();
    show_options(*component);
    ImGui::Unindent();
    ImGui::Spacing();
}