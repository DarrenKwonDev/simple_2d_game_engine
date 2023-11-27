#include <iostream>

#include "Game.h"

using namespace std;

Game::Game() {
    cout << "Game constructor called" << endl;
}

Game::~Game() {
    cout << "Game deconstructor called" << endl;
}

void Game::Initialize() {
}

void Game::Run() {
    while (true) {
        Game::ProcessInput();
        Game::Update();
        Game::Render();
    }
}

void Game::ProcessInput() {
}

void Game::Update() {
}

void Game::Render() {
}

void Game::Destroy() {
}
