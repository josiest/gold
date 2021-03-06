#pragma once

// frameworks
#include <SDL.h>
#include <SDL_ttf.h>

// data types
#include <cstdint>
#include <cstddef> // std::size_t
#include <string>

// templates data-types and concepts
#include <concepts>
#include "gold/result.hpp"

namespace au {

class iwidget {
public:
    /** Get the bounds of this widget. */
    virtual SDL_Rect bounds() const = 0;

    /** Render this widget. */
    virtual void render(SDL_Renderer * renderer) = 0;

    /** Determine if this widget is active. */
    virtual bool is_active() const = 0;

    /** Activate this widget.
     *
     * Widgets are intialized as active.
     */
    virtual void activate() = 0;

    /** Deactivate this widget. */
    virtual void deactivate() = 0;

    /** Get the id of this widget. */
    virtual std::size_t id() const = 0;

    virtual ~iwidget() {}
};

class imonochrome {
public:
    /** Get the color of this object */
    virtual SDL_Color get_color() const = 0;

    /** Set the color of this object */
    virtual void set_color(SDL_Color const & color) = 0;

    virtual ~imonochrome() {}
};

class itext_widget : public iwidget {
public:
    /** Get the text associated with this widget. */
    virtual std::string get_text() const = 0;

    /** Set the text associated with this widget. */
    virtual void set_text(std::string const & text) = 0;

    virtual ~itext_widget() {}
};

template<class factory_t>
concept text_widget_factory =

    requires(factory_t & factory, SDL_Renderer * renderer,
             std::string const & text, SDL_Rect const & bounds) {

    { factory.make_text_widget(renderer, text, bounds) } ->
        std::same_as<result<itext_widget *>>;
};

template<class widget_subclass>
    requires std::derived_from<widget_subclass, iwidget>

iwidget * as_widget(widget_subclass * widget)
{
    return dynamic_cast<iwidget *>(widget);
}
}
