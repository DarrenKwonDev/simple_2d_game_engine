#include <iostream>

#include <SDL2/SDL.h>

#include "Game.h"

using namespace std;

Game::Game() {
    cout << "Game constructor called" << endl;
    this->mIsRunning = false;
}

Game::~Game() {
    cout << "Game deconstructor called" << endl;
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cerr << "Error initializing SDL" << endl;
        return;
    }

    // create window
    this->mWindow = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_BORDERLESS);

    if (!mWindow) {
        cerr << "Error creating SDL window" << endl;
        return;
    }

    // create renderer
    this->mRenderer = SDL_CreateRenderer(
        mWindow,
        -1, // default.
        0);
    if (!mRenderer) {
        cerr << "Error creating SDL renderer" << endl;
        return;
    }

    this->mIsRunning = true;
}

// main game loop three logics : ProcessInput, Update, Render.
void Game::ProcessInput() {
    SDL_Event sdlEvent;

    // Poll for currently pending events
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
        // user press 'x' window close btn.
        case SDL_QUIT:
            this->mIsRunning = false;
            break;

        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                this->mIsRunning = false;
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
}

void Game::Run() {
    while (this->mIsRunning) {
        Game::ProcessInput();
        Game::Update();
        Game::Render();
    }
}

// clean up
void Game::Destroy() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
