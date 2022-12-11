#include "imgui/imgui.h"

template<gold::editor_option component_type>
requires gold::has_public_name<component_type>
inline void
gold::show_component_options(entt::registry & widgets, entt::entity widget)
{
    static bool is_open = true;
    auto * component = widgets.try_get<component_type>(widget);
    if (not component) {
        is_open = false;
        return;
    }
    else {
        is_open = true;
    }
    std::string_view constexpr name =
        component_info<component_type>::public_name;

    auto const header_flags = ImGuiTreeNodeFlags_Leaf;
    if (ImGui::CollapsingHeader(name.data(), &is_open, header_flags)) {
        ImGui::Spacing();
        ImGui::Indent();
        show_options(*component);
        ImGui::Unindent();
    }
    ImGui::Spacing();
    if (not is_open) {
        widgets.erase<component_type>(widget);
    }
}