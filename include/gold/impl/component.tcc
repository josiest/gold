#include "imgui/imgui.h"

template<gold::editor_option component_type>
requires gold::has_public_name<component_type>
inline void
gold::show_component_options(entt::registry & widgets, entt::entity widget)
{
    if (auto * component = widgets.try_get<component_type>(widget))
    {
        std::string_view constexpr name =
            component_info<component_type>::public_name;
        ImGui::Text("%s", name.data());
        ImGui::Spacing();

        ImGui::Indent();
        show_options(*component);
        ImGui::Unindent();
    }
}