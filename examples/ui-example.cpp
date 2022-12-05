// frameworks
#include "ion/system.hpp"
#include "ion/ui.hpp"
#include <SDL2/SDL.h>

// data types and structure
#include <string>
#include <vector>
#include <unordered_map>

// serialization and i/o
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <iostream>

// algorithms
#include <ranges>
#include <algorithm>
namespace ranges = std::ranges;
namespace views = std::views;

namespace paths {
static std::filesystem::path const assets =
    std::filesystem::canonical("../assets");

static std::filesystem::path const system_config = assets/"system.yaml";
}
void print_error(std::string const & message) {
    std::cout << message << "\n\n";
}

namespace global {
bool show_demo = false;
}

void render(SDL_Window *)
{
    if (global::show_demo) {
        ImGui::ShowDemoWindow(&global::show_demo);
    }
    if (not ImGui::NewWindow()) {
        ImGui::End();
        return;
    }

    // draw example widget centered
    ImVec4 const lavender{0.429f, 0.160f, 0.480f, 0.540f};
    ImGui::PushStyleColor(ImGuiCol_ChildBg, lavender);

    ImVec2 const size{ 100.f, 100.f };
    float const avail_width = ImGui::GetContentRegionAvail().x;
    float const x_offset = (avail_width - size.x)/2.f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() +  x_offset);

    ImGui::BeginChild("example widget", size);
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::End();
}

template<std::uint16_t mask>
inline constexpr bool mask_contains(std::uint16_t mod)
{
    std::uint16_t constexpr all = KMOD_SHIFT | KMOD_ALT | KMOD_CTRL;
    mod &= all;
    return (mod & mask) == mod;
}

template<std::uint16_t mask>
inline constexpr bool has_mask(std::uint16_t mod)
{
    return (mod & mask) != KMOD_NONE;
}

void toggle_demo(SDL_Keysym const & sym)
{
    auto constexpr demo_mask = KMOD_CTRL;
    bool const can_toggle = mask_contains<demo_mask>(sym.mod) and
                            has_mask<KMOD_CTRL>(sym.mod);

    if (sym.sym == SDLK_d and can_toggle) {
        global::show_demo = not global::show_demo;
    }
}

int main()
{
    std::vector<YAML::Exception> yaml_errors;
    auto system = ion::system::from_config(paths::system_config, yaml_errors);

    ranges::for_each(yaml_errors | views::transform(&YAML::Exception::what),
                     print_error);
    if (not system) {
        print_error(system.error());
        return EXIT_FAILURE;
    }

    system->on_render().connect<&render>();
    system->on_keydown().connect<&toggle_demo>();
    system->start();
    return EXIT_SUCCESS;
}