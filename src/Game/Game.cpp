#include <fstream>
#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_sdl.h"

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
#include "Components/CameraFollowComponent.h"
#include "Components/HealthComponent.h"
#include "Components/KeyboardControlComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/TextLabelComponent.h"
#include "Events/KeyPressedEvent.h"
#include "Game.h"
#include "Macros.h"
#include "SDL2/SDL_keycode.h"
#include "Systems/CameraMovementSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/KeyboardControlSystem.h"
#include "Systems/ProjectileEmitSystem.h"
#include "Systems/ProjectileLifecycleSystem.h"
#include "Systems/RenderColliderSystem.h"
#include "Systems/RenderHealthbarSystem.h"
#include "Systems/RenderTextSystem.h"

int Game::mWindowWidth;
int Game::mWindowHeight;
int Game::mMapWidth;
int Game::mMapHeight;

Game::Game() {
    mIsRunning = false;
    mIsDebug = false;
    mRegistry = std::make_unique<Registry>();
    mAssetStore = std::make_unique<AssetStore>();
    mEventBus = std::make_unique<EventBus>();

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

    if (TTF_Init() != 0) {
        Logger::Err("Error initializing SDL TTF");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    mWindowWidth = displayMode.w;
    mWindowHeight = displayMode.h;

    // create window
    mWindow = SDL_CreateWindow(
        NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWindowWidth, mWindowHeight, SDL_WINDOW_BORDERLESS);

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

    // FIXME: when engine crashed, window is not restored to normal state.
    // SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);

    // init imgui
    ImGui::CreateContext();
    ImGuiSDL::Initialize(mRenderer, mWindowWidth, mWindowHeight);

    // set camera
    Logger::Log("camera initialized");
    mCamera.x = 0;
    mCamera.y = 0;
    mCamera.w = mWindowWidth;
    mCamera.h = mWindowHeight;

    // game loop running
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

        // pass event to imgui
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO& io = ImGui::GetIO();

        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        switch (sdlEvent.type) {
        // user press 'x' window close btn.
        case SDL_QUIT:
            mIsRunning = false;
            break;

        case SDL_KEYDOWN:

            mEventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);

            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                mIsRunning = false;
            };
            if (sdlEvent.key.keysym.sym == SDLK_d) {
                mIsDebug = !mIsDebug;
            }
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
    mRegistry->AddSystem<RenderColliderSystem>();
    mRegistry->AddSystem<DamageSystem>();
    mRegistry->AddSystem<KeyboardControlSystem>();
    mRegistry->AddSystem<CameraMovementSystem>();
    mRegistry->AddSystem<ProjectileEmitSystem>();
    mRegistry->AddSystem<ProjectileLifecycleSystem>();
    mRegistry->AddSystem<RenderTextSystem>();
    mRegistry->AddSystem<RenderHealthBarSystem>();

    // add texture
    mAssetStore->AddTexture(mRenderer, "tank-image", "./assets/images/tank-panther-right.png");
    mAssetStore->AddTexture(mRenderer, "truck-image", "./assets/images/truck-ford-right.png");
    mAssetStore->AddTexture(mRenderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    mAssetStore->AddTexture(mRenderer, "radar-image", "./assets/images/radar.png");
    mAssetStore->AddTexture(mRenderer, "bullet-image", "./assets/images/bullet.png");

    // add fonts
    mAssetStore->AddFont("charriot-font-16", "./assets/fonts/charriot.ttf", 16);
    mAssetStore->AddFont("arial-font-16", "./assets/fonts/arial.ttf", 16);

    // tilemap
    mAssetStore->AddTexture(mRenderer, "tilemap-image", "./assets/tilemaps/jungle.png");
    int tileSize = 32; // 32 pixel
    double tileScale = 2.0;
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
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
                                                  glm::vec2(tileScale, tileScale),
                                                  0.0);

            // tilemap-image의 x, y 좌표에 의해 렌더링 함.
            // tilemap-image의 배치는 다음과 같음.
            // 0 1 2 3 4 5 6 7 8 9
            // 10 11 12 13 14 15 16 17 18 19
            // 20 21 22 23 24 25 26 27 28 29
            tile.AddComponent<SpriteComponent>(
                "tilemap-image", tileSize, tileSize, ZIndex::Background, false, srcRectX, srcRectY);
        }
    }

    mapFile.close();

    mMapWidth = mapNumCols * tileSize * tileScale;
    mMapHeight = mapNumRows * tileSize * tileScale;

    // create entity and add component
    Entity chopper = mRegistry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, ZIndex::Player);
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<AnimationComponent>(2, 15, true); // it has 2 frame and render five frame per sec
    chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0, -200),
                                                   glm::vec2(200, 0),
                                                   glm::vec2(0, 200),
                                                   glm::vec2(-200, 0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);
    // mRepeatFrequencyMS가 0인 것은, 발사를 막는 효과를 내도록 처리함.
    chopper.AddComponent<ProjectileEmitterComponent>(
        glm::vec2(250.0, 250.0), DO_NOT_AUTO_RE_EMIT_PROJECTILE_FREQ, 10000, 10, true);

    Entity radar = mRegistry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(mWindowWidth - 74, 32), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, ZIndex::UI, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity tank = mRegistry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(400.0, 550.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(-30.0, 00.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, ZIndex::Enemy);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 1000, 10000, 10, false);
    tank.AddComponent<HealthComponent>(100);

    Entity truck = mRegistry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(100.0, 350.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 00.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, ZIndex::Enemy);
    truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 1000, 2000, 10, false);
    truck.AddComponent<HealthComponent>(100);

    Entity label = mRegistry->CreateEntity();
    SDL_Color green = {0, 255, 0};
    label.AddComponent<TextLabelComponent>(glm::vec2(100, 100), "CHOPPER 1.0", "arial-font-16", green, true);
}

// one time setup
void Game::Setup() {
    Game::LoadLevel(1);
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

    // clear all event cb
    mEventBus->Reset();

    // re subscribe
    mRegistry->GetSystem<DamageSystem>().SubscribeToEvents(mEventBus);
    mRegistry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(mEventBus);
    mRegistry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(mEventBus);

    // system update
    mRegistry->GetSystem<MovementSystem>().Update(deltaTime);
    mRegistry->GetSystem<AnimationSystem>().Update();
    mRegistry->GetSystem<CollisionSystem>().Update(mEventBus);
    mRegistry->GetSystem<CameraMovementSystem>().Update(mCamera);
    mRegistry->GetSystem<ProjectileEmitSystem>().Update(mRegistry);
    mRegistry->GetSystem<ProjectileLifecycleSystem>().Update();

    // system update를 마친후 생성, 삭제 대기 중인 entity를
    // 다음 tick update에 반영하기 위해 system에 등록
    mRegistry->Update();
}

// game loop 진행 상 Game::Update 직후 Game::Render 함수 호출.
// 결과적으로 per frame마다 호출됨.
// Game::Update에서 변경된 state를 기반으로 그래픽 표현.
void Game::Render() {
    SDL_SetRenderDrawColor(mRenderer, 153, 153, 153, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mRenderer); // clears the entire rendering target

    mRegistry->GetSystem<RenderSystem>().Update(mRenderer, mAssetStore, mCamera);
    mRegistry->GetSystem<RenderTextSystem>().Update(mRenderer, mAssetStore, mCamera);
    mRegistry->GetSystem<RenderHealthBarSystem>().Update(mRenderer, mAssetStore, mCamera);

    if (mIsDebug) {
        mRegistry->GetSystem<RenderColliderSystem>().Update(mRenderer, mCamera);

        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }

    // present. (as double buffered renderer, swap back/front buffer)
    SDL_RenderPresent(mRenderer);
}

// clean up
void Game::Destroy() {
    ImGui::DestroyContext();
    ImGuiSDL::Deinitialize();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
