#include <iostream>

#include <SDL2/SDL.h>

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

void Game::Run() {
    Game::Setup();
    while (mIsRunning) {
        Game::ProcessInput();
        Game::Update();
        Game::Render();
    }
}

void Game::Setup() {
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

void Game::Update() {
}

void Game::Render() {
    SDL_SetRenderDrawColor(mRenderer, 21, 21, 21, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mRenderer); // clears the entire rendering target

    // present. (as double buffered renderer, swap back/front buffer)
    SDL_RenderPresent(mRenderer);
}

// clean up
void Game::Destroy() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
