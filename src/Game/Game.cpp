#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "glm/glm.hpp"

#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "Game.h"

using namespace std;

Game::Game() {
    mIsRunning = false;
    mRegistry = std::make_unique<Registry>();
    Logger::Log("Game constructor called");
}

Game::~Game() {
    Logger::Log("Game deconstructor called");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error initializing SDL");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    mWindowWidth = displayMode.w;
    mWindowHeight = displayMode.h;

    // create window
    mWindow = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        mWindowWidth, mWindowHeight,
        SDL_WINDOW_BORDERLESS);

    if (!mWindow) {
        Logger::Err("Error creating SDL window");
        return;
    }

    // create renderer
    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,                                                  // 기본 그래픽 드라이버 사용
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC // GPU 사용 + Vsync
    );

    if (!mRenderer) {
        Logger::Err("Error creating SDL renderer");
        return;
    }

    // SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);

    mIsRunning = true;
}

void Game::RunGameLoop() {
    Game::Setup();
    while (mIsRunning) {
        Game::ProcessInput();
        Game::Update();
        Game::Render();
        // blocking delay until we reach target frame time in milliseconds
    }
}

// main game loop three logics : ProcessInput, Update, Render.
void Game::ProcessInput() {
    SDL_Event sdlEvent;

    // Poll for currently pending events
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
        // user press 'x' window close btn.
        case SDL_QUIT:
            mIsRunning = false;
            break;

        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                mIsRunning = false;
            };
            break;

        default:
            break;
        }
    };
}

// one time setup
void Game::Setup() {

    Entity tank = mRegistry->CreateEntity();

    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 0.0));

    tank.RemoveComponent<TransformComponent>();
}

// called every frame
void Game::Update() {
    // https://wiki.libsdl.org/SDL2/SDL_Delay
    // 지정된 시간 동안 현재 thread에서 벗어나 다른 작업을 하도록
    // context switching을 의도하고 OS scheduler에게 위임.
    int timeToWait = MILLISEC_PER_FRAME - (SDL_GetTicks() - millisecPrevFrame);

    // 너무 빠를 경우 대기.
    if (timeToWait > 0 && timeToWait <= MILLISEC_PER_FRAME) {
        SDL_Delay(timeToWait);
    }

    // deltaTime = 현재시간 - 이전프레임의시간;
    double deltaTime = (SDL_GetTicks() - millisecPrevFrame) / 1000.0;

    millisecPrevFrame = SDL_GetTicks();

    // TODO: MovementSystem.Update();
    // CollisionSystem.Update();
    // DamageSystem.Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(mRenderer, 153, 153, 153, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mRenderer); // clears the entire rendering target

    // TODO: render game object

    // present. (as double buffered renderer, swap back/front buffer)
    SDL_RenderPresent(mRenderer);
}

// clean up
void Game::Destroy() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
