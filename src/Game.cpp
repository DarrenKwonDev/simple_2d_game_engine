#include <iostream>

#include <SDL2/SDL.h>

#include "Game.h"

using namespace std;

Game::Game() {
    cout << "Game constructor called" << endl;
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
}

void Game::Run() {
    // while (true) {
    //     Game::ProcessInput();
    //     Game::Update();
    //     Game::Render();
    // }
}

void Game::ProcessInput() {
}

void Game::Update() {
}

void Game::Render() {
}

void Game::Destroy() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
