#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

bool running = false;

int main(int argc, char* argv[]) {
    if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) | TTF_Init()) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
    }
    
    // Create a window for main loop
    // [ - IMPORTANT CHANGE - ] Change name for engine by reading from a resource file
    SDL_Window* window = SDL_CreateWindow("Vortex Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    running = true;

    // Create an SDL Event
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}