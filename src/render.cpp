#include "gold/render.hpp"
#include "gold/background_color.hpp"
#include "gold/size.hpp"
#include "gold/layout.hpp"

#include "imgui/imgui.h"
#include <entt/entity/registry.hpp>

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
