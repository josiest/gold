#pragma once
#include <entt/entity/registry.hpp>

class ImVec2;
inline namespace gold {
namespace align {
enum class horizontal;
enum class vertical;
};
class size;

/**
 * \brief Horizontally align the imgui cursor to the desired setting
 *
 * \param size          size component settings on widget. Used to calculate the
 *                      x position to render the widget.
 * \param halign        desired horizontal alignment
 * \param desired_size  the actual render size settings for imgui. It's assumed
 *                      this will be directly passed to ImGui::BeginChild
 *
 * If halign is fill, the current desired_size.x will be overwritten so that
 * ImGui will fill the entire available width when rendering the widget.
 */
void align_cursor(gold::size size, align::horizontal halign, ImVec2 & desired_size);

/**
 * \brief Vertically align the imgui cursor to the desired setting
 *
 * \param size          size component settings on widget. Used to calculate the
 *                      y position to render the widget.
 * \param valign        desired vertical alignment
 * \param desired_size  the actual render size settings for imgui. It's assumed
 *                      this will be directly passed to ImGui::BeginChild
 *
 * If halign is fill, the current desired_size.y will be overwritten so that
 * ImGui will fill the entire available height when rendering the widget.
 */
void align_cursor(gold::size size, align::vertical valign, ImVec2 & desired_size);

/** Render a widget based on what components it has. */
void render(entt::registry & widgets, entt::entity widget);
}