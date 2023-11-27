#pragma once

#include <SDL2/SDL.h>

class Game {
private:
    bool mIsRunning; // if window and renderer set OK.
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

public:
    Game();
    virtual ~Game();

    void Initialize();
    void RunGameLoop();
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();

    int mWindowWidth;
    int mWindowHeight;
};
