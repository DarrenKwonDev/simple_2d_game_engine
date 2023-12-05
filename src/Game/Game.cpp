#include <fstream>
#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Components/AnimationComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "Components/BoxColliderComponent.h"
#include "Game.h"
#include "Systems/CollisionSystem.h"

using namespace std;

Game::Game() {
    mIsRunning = false;
    mRegistry = std::make_unique<Registry>();
    mAssetStore = std::make_unique<AssetStore>();
    Logger::Log("Game constructor called");
}

Game::~Game() { Logger::Log("Game deconstructor called"); }

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
    mWindow = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWindowWidth, mWindowHeight,
                               SDL_WINDOW_BORDERLESS);

    if (!mWindow) {
        Logger::Err("Error creating SDL window");
        return;
    }

    // create renderer
    mRenderer = SDL_CreateRenderer(mWindow,
                                   -1, // 기본 그래픽 드라이버 사용
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

void Game::LoadLevel(int level) {
    // add system
    mRegistry->AddSystem<MovementSystem>();
    mRegistry->AddSystem<RenderSystem>();
    mRegistry->AddSystem<AnimationSystem>();
    mRegistry->AddSystem<CollisionSystem>();

    // add texture
    mAssetStore->AddTexture(mRenderer, "tank-image", "./assets/images/tank-panther-right.png");
    mAssetStore->AddTexture(mRenderer, "truck-image", "./assets/images/truck-ford-right.png");
    mAssetStore->AddTexture(mRenderer, "chopper-image", "./assets/images/chopper.png");
    mAssetStore->AddTexture(mRenderer, "radar-image", "./assets/images/radar.png");

    // tilemap
    mAssetStore->AddTexture(mRenderer, "tilemap-image", "./assets/tilemaps/jungle.png");
    int tileSize = 32; // 32 pixel
    double tileScale = 1.0;
    int mapNumCols = 25;
    int mapNumRows = 20;

    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");
    if (!mapFile) {
        // TODO: error handling
        exit(1);
    }

    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore(); // ','는 버려야 하므로.

            Entity tile = mRegistry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
                                                  glm::vec2(tileScale, tileScale), 0.0);

            // tilemap-image의 x, y 좌표에 의해 렌더링 함.
            // tilemap-image의 배치는 다음과 같음.
            // 0 1 2 3 4 5 6 7 8 9
            // 10 11 12 13 14 15 16 17 18 19
            // 20 21 22 23 24 25 26 27 28 29
            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
        }
    }

    mapFile.close();

    // create entity and add component
    Entity chopper = mRegistry->CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 15, true); // it has 2 frame and render five frame per sec

    Entity radar = mRegistry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(mWindowWidth - 74, 32), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity tank = mRegistry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(200.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(-30.0, 00.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));

    Entity truck = mRegistry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 00.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 10);
    truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));

    // tank.RemoveComponent<TransformComponent>();
}

// one time setup
void Game::Setup() { Game::LoadLevel(1); }

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
    mRegistry->GetSystem<AnimationSystem>().Update();
    mRegistry->GetSystem<CollisionSystem>().Update();

    // present. (as double buffered renderer, swap back/front buffer)
    SDL_RenderPresent(mRenderer);
}

// clean up
void Game::Destroy() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
