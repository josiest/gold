// frameworks
#include <SDL.h>
#include <SDL_ttf.h>
#include <gold/gold.h>

// constants
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE

// i/o
#include <iostream>
#include <filesystem> // fs::path, fs::absolute

// aliases 
namespace fs = std::filesystem;

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

    // initialize ttf
    if (TTF_Init() < 0) {
        std::cout << "Couldn't initialize ttf! " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Load a font
    fs::path const font_path = fs::absolute("../assets/fonts/DejaVuSans.ttf");
    TTF_Font * dejavu_sans = TTF_OpenFont(font_path.c_str(), 100);
    if (not dejavu_sans) {
        std::cout << "Couldn't open font! " << TTF_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        TTF_Quit(); SDL_Quit();
        return EXIT_FAILURE;
    }

    // Create the text for the button
    SDL_Color const black{0, 0, 0, 0xff};
    SDL_Surface * text_surface = 
        TTF_RenderText_Solid(dejavu_sans, "Click Me!", black);
    if (not text_surface) {
        std::cout << "Couldn't render text! " << TTF_GetError() << std::endl;
        TTF_CloseFont(dejavu_sans); TTF_Quit();
        SDL_DestroyWindow(window); SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // convert the text surface to a texture
    SDL_Texture * rendered_text =
        SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);
    if (not rendered_text) {
        std::cout << "Couldn't convert text surface to txture! "
                  << SDL_GetError() << std::endl;
        TTF_CloseFont(dejavu_sans); TTF_Quit();
        SDL_DestroyWindow(window); SDL_DestroyRenderer(renderer);
        SDL_Quit();
    }

    // create a button
    SDL_Rect const button_bounds{50, 50, 300, 100};
    SDL_Color const white{0xff, 0xff, 0xff, 0xff};
    au::button simple_button(button_bounds, black, white, 10,
                             rendered_text, 5, 5);

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

    SDL_DestroyTexture(rendered_text);
    TTF_CloseFont(dejavu_sans);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
