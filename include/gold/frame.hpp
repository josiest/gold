#pragma once

// frameworks and interfaces
#include <SDL.h>
#include "gold/widget.hpp"
#include <tl/expected.hpp>

// data types
#include <cstdint>
#include <string>
#include <filesystem>

namespace au {

class frame {
public:
    frame(SDL_Renderer * renderer, SDL_Rect const & bounds,
          std::uint32_t button_height, std::uint32_t padding);

    frame(SDL_Renderer * renderer, int x, int y, std::uint32_t w, std::uint32_t h,
          std::uint32_t button_height, std::uint32_t padding);

    result<itext_widget *> produce_text_widget(text_widget_factory auto & factory,
                                               std::string const & text)
    {
        // compute the widget bounds with frame padding
        SDL_Rect const bounds{
            _next.x, _next.y, _bounds.w - 2*_padding, _button_height
        };
        _next.y += _button_height + _padding;

        // produce the widget from the factory and add it if it succesffuly initialized
        result<itext_widget *> widget =
            factory.make_text_widget(_renderer, text, bounds);
        if (widget) {
            _widgets.push_back(dynamic_cast<iwidget *>(*widget));
        }
        return widget;
    }

    /** Render all widgets in the frame. */
    void render();

    /** Load a frame from a configuration file.
     *
     * \param renderer used to render widgets in the frame
     * \param path the yaml configuration file
     */
    static result<frame> from_file(SDL_Renderer * renderer,
                                   std::filesystem::path const & path);

    inline bool is_active() const { return _active; }
    inline void activate();
    inline void deactivate();
private:
    SDL_Renderer * _renderer;
    SDL_Rect _bounds;
    int _button_height;
    int _padding;
    SDL_Point _next;
    std::vector<iwidget *> _widgets;
    bool _active;
};
}
