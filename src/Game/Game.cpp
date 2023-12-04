#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "glm/glm.hpp"

#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "Game.h"

using namespace std;

Game::Game() {
    mIsRunning = false;
    mRegistry = std::make_unique<Registry>();
    mAssetStore = std::make_unique<AssetStore>();
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

    mRegistry->AddSystem<MovementSystem>();
    mRegistry->AddSystem<RenderSystem>();

    mAssetStore->AddTexture(mRenderer, "tank-image", "./assets/images/tank-panther-right.png");
    mAssetStore->AddTexture(mRenderer, "truck-image", "./assets/images/truck-ford-right.png");

    Entity tank = mRegistry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(2.0, 2.0), 45.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(80.0, 00.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32);

    Entity truck = mRegistry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(1.0, 1.0), 0.5);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32);

    // tank.RemoveComponent<TransformComponent>();
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

    // system update
    mRegistry->GetSystem<MovementSystem>().Update(deltaTime);

    // system update를 마친후 생성, 삭제 대기 중인 entity를
    // 다음 tick update에 반영하기 위해 system에 등록
    // 이 함수의 특성상 최하단에 호출되어야 함
    mRegistry->Update();
}

// game loop 진행 상 Game::Update 직후 Game::Render 함수 호출.
// 결과적으로 per frame마다 호출됨.
// Game::Update에서 변경된 state를 기반으로 그래픽 표현.
void Game::Render() {
    SDL_SetRenderDrawColor(mRenderer, 153, 153, 153, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mRenderer); // clears the entire rendering target

    mRegistry->GetSystem<RenderSystem>().Update(mRenderer, mAssetStore);

    // present. (as double buffered renderer, swap back/front buffer)
    SDL_RenderPresent(mRenderer);
}

// clean up
void Game::Destroy() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
