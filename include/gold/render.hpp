#pragma once
#include <entt/entity/registry.hpp>

class ImVec2;
namespace gold {
namespace align {
enum class horizontal;
enum class vertical;
};
class size;

void align_cursor(gold::size size, align::horizontal halign, ImVec2 & desired_size);
void align_cursor(gold::size size, align::vertical valign, ImVec2 & desired_size);
void render(entt::registry & widgets, entt::entity widget);
}