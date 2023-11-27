#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "glm/glm.hpp"

#include "Game.h"

using namespace std;

Game::Game() {
    cout << "Game constructor called" << endl;
    mIsRunning = false;
}

Game::~Game() {
    cout << "Game deconstructor called" << endl;
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cerr << "Error initializing SDL" << endl;
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
        cerr << "Error creating SDL window" << endl;
        return;
    }

    // create renderer
    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,                                                  // 기본 그래픽 드라이버 사용
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC // GPU 사용 + Vsync
    );

    if (!mRenderer) {
        cerr << "Error creating SDL renderer" << endl;
        return;
    }

    SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);

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

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

// one time setup
void Game::Setup() {
    playerPosition = glm::vec2(10.0, 20.0);
    playerVelocity = glm::vec2(100.0, 0.0);
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

    // playerVelocity.x pixel per sec
    playerPosition.x += (playerVelocity.x * deltaTime);
    playerPosition.y += (playerVelocity.y * deltaTime);
}

void Game::Render() {
    SDL_SetRenderDrawColor(mRenderer, 153, 153, 153, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mRenderer); // clears the entire rendering target

    // 경로에 주의.
    // 상대 경로를 인식하지 못함.
    // 현재 작업 디렉토리(Current Working Directory)를 기준으로 작성해야 함.
    SDL_Surface* surface = IMG_Load((string(SDL_GetBasePath()) + "/assets/images/tank-tiger-right.png").c_str());
    if (surface == NULL) {
        cerr << "Image Load Error " << SDL_GetError() << endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    if (texture == NULL) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        return;
    }

    // 이미 텍스처로 변환된 후에는 SDL_Surface 객체가 더 이상 필요하지 않으므로 메모리를 해제합니다.
    SDL_FreeSurface(surface);

    SDL_Rect dstRect = {
        // x, y
        static_cast<int>(playerPosition.x),
        static_cast<int>(playerPosition.y),
        32, 32 // w, h
    };
    SDL_RenderCopy(mRenderer, texture, NULL, &dstRect);

    // 사용이 끝난 후에는 SDL_Texture 객체 정리.
    SDL_DestroyTexture(texture);

    // present. (as double buffered renderer, swap back/front buffer)
    SDL_RenderPresent(mRenderer);
}

// clean up
void Game::Destroy() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
