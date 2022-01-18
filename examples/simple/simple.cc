// frameworks
#include <SDL.h>
#include <gold/gold.h>

// constants
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE

// i/o
#include <iostream>

int main()
{
    // initialize sdl
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Unable to initialize SDL! " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    // create a simple sdl window
    SDL_Window * window = SDL_CreateWindow(
            "Simple Example",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480, 0
            );
    if (not window) {
        std::cout << "Couldn't create a window! " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // create a basic hardware renderer
    SDL_Renderer * renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED
            );
    if (not renderer) {
        std::cout << "Couldn't create a renderer! " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // create a button
    SDL_Rect const button_bounds{50, 50, 300, 100};
    SDL_Color const black{0, 0, 0, 0xff};
    SDL_Color const white{0xff, 0xff, 0xff, 0xff};
    au::button simple_button(button_bounds, black, white, 5);

    bool has_quit = false;
    while (not has_quit) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                has_quit = true;
            }
        }

        // clear the screen white
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);

        // draw the button and update the screen
        simple_button.render(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
