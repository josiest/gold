# gold gui

An _extremely_ simple gui framework for SDL2.

## Installing

### Requirements

In order to install and use gold, there are a few requirements:

- [`SDL2`](https://www.libsdl.org/): Since this is a framework for SDL2, you
  should have it installed. If you don't have it installed, you can
  [find instructions here](https://wiki.libsdl.org/Installation)

- [`SDL_ttf`](https://github.com/libsdl-org/SDL_ttf): Standard SDL2 library for
  rendering text

- [`expected`](https://github.com/TartanLlama/expected): An implementation of
  the proposed `expected` interface - a convenient alternative to exceptions

- [`cpp-yaml`](https://github.com/jbeder/yaml-cpp): gui elements have a lot of
  parameters associated with them - serialization makes instantiation simpler

```bash
git clone <url> && cd gold
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

## Why the name gold?

Because gui means "expensive" in Chinese
