#pragma once

// engine
#include "glad/glad.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "ion/try.hpp"

// frameworks
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

// data types and handles
#include <string>
#include <filesystem>

// serialization
#include "konbu/konbu.h"
#include <sstream>
#include <iterator>
#include <regex>

// events
#include <vector>
#include <functional>

#include <ranges>
#include <algorithm>

namespace ion {

using flagmap = std::unordered_map<std::string, std::uint32_t>;

class system {
public:
    system() = delete;
    system(system const &) = delete;
    system & operator=(system const &) = delete;

    system(system && temp);
    system & operator=(system && temp);
    ~system();

    //
    // Deserialization
    //

    /**
     * \brief initialize an ion::system from yaml settings
     * \tparam error_output allocator-aware container of yaml-exceptions
     *
     * \param config        input for the various system settings
     * \param yaml_errors   write any parsing errors to
     *
     * \return a system if no SDL_Errors were encountered, otherwise the
     *         relevant SDL_Error.
     *
     * "Various system settings" include settings for:
     * - system -> ion::init_params     how to initialize SDL
     * - window -> ion::window_params   how to create the window
     * - opengl -> ion::opengl_params   how to initialize OpenGL
     * - imgui -> ion::imgui_params     how to initialize ImGui
     */
    template<std::ranges::output_range<YAML::Exception> error_output>
    [[nodiscard]] static std::expected<system, std::string>
    from_config(YAML::Node const & config, error_output & yaml_errors);

    /**
     * \brief initialize an ion::system system from loading a config file
     *
     * \tparam error_output allocator-aware container of yaml-exceptions
     *
     * \param config_path   where to find the system config settings
     * \param yaml_errors   write any parsing errors to
     *
     * \return the loaded system if no SDL_Errors were encountered, otherwise
     *         the relevant SDL_Error
     */
    template<std::ranges::output_range<YAML::Exception> error_output>
    [[nodiscard]] static std::expected<system, std::string>
    from_config(std::filesystem::path const & config_path,
                error_output & yaml_errors);

    //
    // Logic
    //

    /** Run the main loop. */
    void start();

    //
    // Subsystems
    //
    entt::registry subsystems;

    /** Register a subsystem. */
    template<class subsystem>
    subsystem & add_subsystem();

    template<class subsystem>
    [[nodiscard]] subsystem & get_subsystem();
    template<class subsystem>
    [[nodiscard]] subsystem const & get_subsystem() const;

    template<class subsystem>
    [[nodiscard]] subsystem * try_get_subsystem();
    template<class subsystem>
    [[nodiscard]] subsystem const * try_get_subsystem() const;

    //
    // Events
    //
    [[nodiscard]] inline auto on_render() { return entt::sink{ render_event }; }
    [[nodiscard]] inline auto on_keydown() { return entt::sink{ keydown_event }; }

    //
    // Utility
    //

    constexpr bool operator!() const;

    //
    //  Members
    //
    [[nodiscard]] inline SDL_Window * window() { return _window; }
    [[nodiscard]] inline SDL_Window const * window() const { return _window; }

    [[nodiscard]] inline void * gl_context() { return _gl_context; }
    [[nodiscard]] inline void const * gl_context() const { return _gl_context; }
    [[nodiscard]] inline entt::entity id() const { return _id; }
private:
    system(SDL_Window * window, SDL_GLContext gl_context);
    bool moved = false;

    // events
    entt::sigh<void(SDL_Window *)> render_event;
    entt::sigh<void(SDL_Keysym const &)> keydown_event;

    SDL_Window * _window;
    SDL_GLContext _gl_context;
    entt::entity _id;
};

struct init_params {
    std::uint32_t subsystems = SDL_INIT_VIDEO;

    inline static flagmap const subsystem_flags{
        { "timer",              SDL_INIT_TIMER },
        { "audio",              SDL_INIT_AUDIO },
        { "video",              SDL_INIT_VIDEO },
        { "joystick",           SDL_INIT_JOYSTICK },
        { "haptic",             SDL_INIT_HAPTIC },
        { "game-controller",    SDL_INIT_GAMECONTROLLER },
        { "events",             SDL_INIT_EVENTS },
        { "everything",         SDL_INIT_EVERYTHING }
    };
};

struct window_params {
    std::string name = "Window";
    int x = SDL_WINDOWPOS_UNDEFINED;
    int y = SDL_WINDOWPOS_UNDEFINED;
    std::uint16_t width = 640u;
    std::uint16_t height = 480u;
    std::uint32_t flags = 0u;

    inline static flagmap const window_flags{
        { "fullscreen",         SDL_WINDOW_FULLSCREEN },
        { "fullscreen-desktop", SDL_WINDOW_FULLSCREEN_DESKTOP },
        { "opengl",             SDL_WINDOW_OPENGL },
        { "vulkan",             SDL_WINDOW_VULKAN },
        { "metal",              SDL_WINDOW_METAL },
        { "hidden",             SDL_WINDOW_HIDDEN },
        { "borderless",         SDL_WINDOW_BORDERLESS },
        { "resizable",          SDL_WINDOW_RESIZABLE },
        { "minimized",          SDL_WINDOW_MINIMIZED },
        { "maximized",          SDL_WINDOW_MAXIMIZED },
        { "input-grabbed",      SDL_WINDOW_INPUT_GRABBED },
        { "allow-high-dpi",     SDL_WINDOW_ALLOW_HIGHDPI },
    };
};

struct opengl_params{
    std::uint16_t double_buffer = 1;
    std::uint16_t depth_size = 24;
    std::uint16_t stencil_size = 8;

    std::uint32_t flags = 0;
    std::uint16_t major_version = 4;
    std::uint16_t minor_version = 2;

    inline static flagmap const opengl_flags{
        { "debug",              SDL_GL_CONTEXT_DEBUG_FLAG },
        { "forward-compatible", SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG },
        { "robust-access",      SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG },
        { "reset-isolation",    SDL_GL_CONTEXT_RESET_ISOLATION_FLAG }
    };
};

struct imgui_params{
    std::string glsl_version = "#version 150";
};

template<class subsystem>
subsystem & system::add_subsystem()
{
    return subsystems.emplace<subsystem>(_id);
}

template<class subsystem>
subsystem & system::get_subsystem()
{
    return subsystems.get<subsystem>(_id);
}

template<class subsystem>
subsystem const & system::get_subsystem() const
{
    return subsystems.get<subsystem>(_id);
}

template<class subsystem>
subsystem * system::try_get_subsystem()
{
    return subsystems.try_get<subsystem>(_id);
}

template<class subsystem>
subsystem const * system::try_get_subsystem() const
{
    return subsystems.try_get<subsystem>(_id);
}
}

namespace konbu {

template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          ion::init_params & params,
          error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    if (not config.IsMap()) {
        YAML::Exception const error{ config.Mark(), "expecting a map" };
        ranges::copy(views::single(error),
                     konbu::back_inserter_preference(errors));
        return;
    }
    using param = ion::init_params;
    if (auto const subsystem_config = config["subsystems"]) {
        std::vector<YAML::Exception> subsystem_errors;

        read_flags(subsystem_config, params.subsystems,
                   param::subsystem_flags, subsystem_errors);
        ranges::transform(subsystem_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_setting("subsystem"));
    }
}

template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          ion::window_params & params,
          error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    if (not config.IsMap()) {
        YAML::Exception const error{
            config.Mark(), "couldn't read window settings: expecting a map"
        };
        ranges::copy(views::single(error),
                     konbu::back_inserter_preference(errors));
        return;
    }
    std::vector<YAML::Exception> window_errors;
    if (auto const name_config = config["name"]) {
        konbu::read(name_config, params.name, window_errors);
        ranges::transform(window_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("name", params.name));
        window_errors.clear();
    }
    if (auto const x_config = config["x"]) {
        konbu::read(x_config, params.x, window_errors);
        ranges::transform(window_errors,
                          konbu::back_inserter_preference(errors) ,
                          konbu::contextualize_param("x", params.x));
        window_errors.clear();
    }
    if (auto const y_config = config["y"]) {
        konbu::read(y_config, params.y, window_errors);
        ranges::transform(window_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("y", params.y));
        window_errors.clear();
    }
    if (auto const width_config = config["width"]) {
        konbu::read(width_config, params.width, window_errors);
        ranges::transform(window_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("width", params.width));
        window_errors.clear();
    }
    if (auto const height_config = config["height"]) {
        konbu::read(height_config, params.height, window_errors);
        ranges::transform(window_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("height", params.height));
        window_errors.clear();
    }
    if (auto const flag_sequence = config["flags"]) {
        konbu::read_flags(flag_sequence, params.flags,
                          ion::window_params::window_flags, window_errors);
        ranges::transform(window_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("flags", params.flags));
        window_errors.clear();
    }
}

template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          ion::opengl_params & params,
          error_output & errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    if (config.IsScalar()) {
        std::vector<YAML::Exception> version_errors;
        konbu::read_version(config, params.major_version, params.minor_version,
                                    version_errors);

        std::stringstream version;
        version << params.major_version << "." << params.minor_version;
        ranges::transform(version_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("version", version.str()));
        return;
    }
    if (not config.IsMap()) {
        YAML::Exception const error{ config.Mark(), "expecting a map" };
        ranges::copy(views::single(error),
                     konbu::back_inserter_preference(errors));
        return;
    }
    if (auto const version_config = config["version"]) {
        std::vector<YAML::Exception> version_errors;
        konbu::read_version(config, params.major_version, params.minor_version,
                                    version_errors);

        std::stringstream version;
        version << params.major_version << "." << params.minor_version;
        ranges::transform(version_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("version", version.str()));
    }
    if (auto const buffer_config = config["double-buffer"]) {
        std::vector<YAML::Exception> buffer_errors;
        konbu::read(buffer_config, params.double_buffer, buffer_errors);
        ranges::transform(buffer_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("double-buffer",
                                                     params.double_buffer));
    }
    if (auto const depth_config = config["depth-size"]) {
        std::vector<YAML::Exception> depth_errors;
        konbu::read(depth_config, params.depth_size, depth_errors);
        ranges::transform(depth_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("depth-size",
                                                     params.depth_size));
    }
    if (auto const stencil_config = config["stencil-size"]) {
        std::vector<YAML::Exception> stencil_errors;
        konbu::read(stencil_config, params.stencil_size, stencil_errors);
        ranges::transform(stencil_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("stencil-size",
                                                     params.stencil_size));
    }
    if (auto const flag_sequence = config["flags"]) {
        std::vector<YAML::Exception> flag_errors;
        konbu::read_flags(flag_sequence, params.flags,
                          ion::opengl_params::opengl_flags, flag_errors);
        ranges::transform(flag_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("flags", params.flags));
    }
}

template<std::ranges::output_range<YAML::Exception> error_output>
void read(YAML::Node const & config,
          ion::imgui_params & params,
          error_output & errors)
{
    namespace ranges = std::ranges;
    if (auto const glsl_config = config["glsl"]) {
        std::vector<YAML::Exception> glsl_errors;
        konbu::read(glsl_config, params.glsl_version, glsl_errors);
        ranges::transform(glsl_errors,
                          konbu::back_inserter_preference(errors),
                          konbu::contextualize_param("glsl",
                                                     params.glsl_version));
    }
}
}

namespace YAML {

void encode_flags(Node & sequence, std::string const & key,
                  std::uint32_t flags, ion::flagmap const & as_flag);

namespace ErrorMsg {

template<std::ranges::range NameRange>
requires std::convertible_to<std::ranges::range_value_t<NameRange>,
                             std::string>
std::string invalid_flagnames(NameRange && flagnames)
{
    std::stringstream err;
    err << "invalid flags: [";
    std::string sep;
    for (std::string const & name : flagnames) {
        err << sep << name;
        sep = ", ";
    }
    err << "]";
    return err.str();
}
}

template<>
struct convert<ion::init_params> {
    static Node encode(const ion::init_params& rhs)
    {
        Node node;
        encode_flags(node, "subsystems", rhs.subsystems,
                     ion::init_params::subsystem_flags);
        return node;
    }
};
template<>
struct convert<ion::window_params> {
    static Node encode(ion::window_params const & rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["x"] = rhs.x;
        node["y"] = rhs.y;
        node["width"] = rhs.width;
        node["height"] = rhs.height;
        encode_flags(node, "flags", rhs.flags,
                     ion::window_params::window_flags);
        return node;
    }
};
template<>
struct convert<ion::opengl_params> {
    static Node encode(ion::opengl_params const & rhs)
    {
        ion::opengl_params const default_params;

        std::stringstream version;
        version << rhs.major_version << "." << rhs.minor_version;

        // input param uses all defaults, so write simplified config
        if (rhs.double_buffer == default_params.double_buffer and
            rhs.depth_size == default_params.depth_size and
            rhs.stencil_size == default_params.stencil_size and
            rhs.flags == default_params.flags) {

            return YAML::Node{ version.str() };
        }
        Node node;
        node["version"] = version.str();
        if (rhs.double_buffer != default_params.double_buffer) {
            node["double-buffer"] = rhs.double_buffer;
        }
        if (rhs.depth_size != default_params.depth_size) {
            node["depth-size"] = rhs.depth_size;
        }
        if (rhs.stencil_size != default_params.stencil_size) {
            node["stencil-size"] = rhs.stencil_size;
        }
        if (rhs.flags != default_params.flags) {
            encode_flags(node, "flags", rhs.flags,
                         ion::opengl_params::opengl_flags);
        }
        return node;
    }
};

template<>
struct convert<ion::imgui_params> {
    static Node encode(ion::imgui_params const & rhs)
    {
        Node node;
        node["glsl-version"] = rhs.glsl_version;
        return node;
    }
};
}

namespace ion::detail {

inline void cleanup(SDL_Window * window,
                    SDL_GLContext gl_context)
{
    if (gl_context) {
        SDL_GL_DeleteContext(gl_context);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

inline std::expected<void, std::string>
init_sdl(const init_params& params)
{
    if (SDL_Init(params.subsystems) != 0) {
        return std::unexpected(SDL_GetError());
    }
    return {};
}

inline std::expected<SDL_Window*, std::string>
load_window(const window_params& params)
{
    auto const &[name, x, y, width, height, _] = params;

    // window _must_ be opengl
    std::uint32_t const flags = params.flags
                              | SDL_WINDOW_OPENGL;

    SDL_Window * window = SDL_CreateWindow(name.c_str(), x, y,
                                           width, height, flags);
    if (not window) {
        return std::unexpected(SDL_GetError());
    }
    return window;
}
inline std::expected<SDL_GLContext, std::string>
load_opengl(const opengl_params& gl, SDL_Window* window)
{
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, gl.double_buffer);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, gl.depth_size);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, gl.stencil_size);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, static_cast<int>(gl.flags));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl.major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl.minor_version);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (not gl_context) {
        return std::unexpected(SDL_GetError());
    }
    SDL_GL_MakeCurrent(window, gl_context);
    if (not gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        return std::unexpected("Couldn't initialize glad");
    }
    return gl_context;
}

inline std::expected<void, std::string>
init_imgui(const imgui_params& params,
           SDL_Window* window,
           SDL_GLContext gl_context)
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontDefault();

    if (not ImGui_ImplSDL2_InitForOpenGL(window, gl_context)) {
        return std::unexpected("Couldn't initialize imgui SDL2");
    }
    if (not ImGui_ImplOpenGL3_Init(params.glsl_version.c_str())) {
        return std::unexpected("Couldn't initialize imgui OpenGL3");
    }
    return {};
}
}

template<std::ranges::output_range<YAML::Exception> error_output>
inline std::expected<ion::system, std::string>
ion::system::from_config(const YAML::Node& config, error_output & yaml_errors)
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    SDL_Window* window;
    SDL_GLContext gl_context;

    ion::init_params init_params;
    if (auto const system_config = config["system"]) {
        std::vector<YAML::Exception> system_errors;
        konbu::read(system_config, init_params, system_errors);
        ranges::transform(system_errors,
                          konbu::back_inserter_preference(yaml_errors),
                          konbu::contextualize_setting("system"));
    }
    TRY_VOID(detail::init_sdl(init_params));

    ion::window_params window_params;
    if (auto const window_config = config["window"]) {
        std::vector<YAML::Exception> window_errors;
        konbu::read(window_config, window_params, window_errors);
        ranges::transform(window_errors,
                          konbu::back_inserter_preference(yaml_errors),
                          konbu::contextualize_setting("window"));
    }
    window = TRY(detail::load_window(window_params),
                 detail::cleanup(nullptr, nullptr));

    ion::opengl_params gl_params;
    if (auto const opengl_config = config["opengl"]) {
        std::vector<YAML::Exception> gl_errors;
        konbu::read(opengl_config, gl_params, gl_errors);
        ranges::transform(gl_errors,
                          konbu::back_inserter_preference(yaml_errors),
                          konbu::contextualize_setting("opengl"));
    }
    gl_context = TRY(detail::load_opengl(gl_params, window),
                     detail::cleanup(window, nullptr));

    ion::imgui_params imgui_params;
    if (auto const imgui_config = config["imgui"]) {
        std::vector<YAML::Exception> imgui_errors;
        konbu::read(imgui_config, imgui_params, imgui_errors);
        ranges::transform(imgui_errors,
                          konbu::back_inserter_preference(yaml_errors),
                          konbu::contextualize_setting("imgui"));
    }
    TRY_VOID(detail::init_imgui(imgui_params, window, gl_context),
             detail::cleanup(window, gl_context));

    return system(window, gl_context);
}

template<std::ranges::output_range<YAML::Exception> error_output>
std::expected<ion::system, std::string>
ion::system::from_config(std::filesystem::path const & config_path,
                         error_output & yaml_errors)
{
    using namespace std::string_literals;
    namespace fs = std::filesystem;

    if (not fs::exists(config_path)) {
        return std::unexpected("No file named "s + config_path.string());
    }
    YAML::Node const config = YAML::LoadFile(config_path.string());
    if (not config) {
        return std::unexpected("Unable to load config at " +
                               config_path.string());
    }
    return from_config(config, yaml_errors);
}