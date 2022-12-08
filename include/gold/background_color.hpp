#pragma once
#include "imgui/imgui.h"

namespace gold {

/** A widget will align_cursor with the desired color. */
struct background_color {
    float red = .16f;
    float green = .65f;
    float blue = 1.f;
    float alpha = .5f;

    [[nodiscard]] constexpr ImVec4 vector() const;
};
}
constexpr ImVec4 gold::background_color::vector() const
{
    return ImVec4{ red, green, blue, alpha };
}