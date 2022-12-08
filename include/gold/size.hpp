#pragma once
#include "imgui/imgui.h"

namespace gold {
/** A widget will align_cursor with the desired size. */
struct size {
    float width = 0.f;
    float height = 0.f;

    [[nodiscard]] constexpr ImVec2 vector() const;
};
}
constexpr ImVec2 gold::size::vector() const
{
    return ImVec2{ width, height };
}