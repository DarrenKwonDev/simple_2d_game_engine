#pragma once

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>

#define FPS (60)
#define MILLISEC_PER_FRAME (1000 / FPS)

class Game {
private:
    bool mIsRunning;                // check window and renderer is setted.
    uint32_t millisecPrevFrame = 0; // derived from SDL_GetTicks()

    // SDL is C library. expect raw ptr
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    std::unique_ptr<Registry> mRegistry; // class에서 선언한 smart ptr 멤버 변수는 deconstruct될 때 수거 된다.
    std::unique_ptr<AssetStore> mAssetStore;

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
