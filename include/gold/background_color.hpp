#pragma once
#include "imgui/imgui.h"

namespace gold {

/** A widget will render with the desired color. */
struct background_color {
    float red = 0.f;
    float green = 0.f;
    float blue = 0.f;
    float alpha = 1.f;

    [[nodiscard]] constexpr ImVec4 vector() const;
};
}
constexpr ImVec4 gold::background_color::vector() const
{
    return ImVec4{ red, green, blue, alpha };
}