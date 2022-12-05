// frameworks
#include "ion/system.hpp"
#include "ion/ui.hpp"
#include <SDL2/SDL.h>

// data types and structure
#include <string>
#include <vector>

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

void render(SDL_Window *)
{
    if (not ImGui::NewWindow()) {
        ImGui::End();
        return;
    }
    ImGui::End();
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
    system->start();
}