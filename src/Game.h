#pragma once

#include <SDL2/SDL.h>

class Game {
private:
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

public:
    Game();
    virtual ~Game();

    void Initialize();
    void Run();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
};
