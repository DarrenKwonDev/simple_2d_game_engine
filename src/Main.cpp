#include "Game/Game.h"

int main(int argc, char* argv[]) {
    Game game; // stack 할당

    game.Initialize();
    game.RunGameLoop();
    game.Destroy();

    return 0;
};
