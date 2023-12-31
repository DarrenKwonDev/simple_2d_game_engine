#pragma once

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include "EventBus/EventBus.h"
#include "sol/sol.hpp"
#include <SDL2/SDL.h>

#define FPS (60)
#define MILLISEC_PER_FRAME (1000 / FPS)

class Game {
private:
    bool mIsRunning; // check window and renderer is setted.
    bool mIsDebug;
    uint32_t millisecPrevFrame = 0; // derived from SDL_GetTicks()

    // SDL is C library. expect raw ptr
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    SDL_Rect mCamera;

    // global lua state
    sol::state mLua;

    std::unique_ptr<Registry> mRegistry; // class에서 선언한 smart ptr 멤버 변수는 deconstruct될 때 수거 된다.
    std::unique_ptr<AssetStore> mAssetStore;
    std::unique_ptr<EventBus> mEventBus;

public:
    Game();
    virtual ~Game();

    static int mWindowWidth;
    static int mWindowHeight;
    static int mMapWidth;
    static int mMapHeight;

    void Initialize();
    void RunGameLoop();
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
};
