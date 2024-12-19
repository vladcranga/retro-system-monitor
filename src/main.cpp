#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: "
                  << SDL_GetError()
                  << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "8Bit System Monitor",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "Failed to create window: "
                  << SDL_GetError()
                  << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Delay(5000);

    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}