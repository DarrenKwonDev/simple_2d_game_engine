#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>

#include "Game/Game.h"
#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "sol/sol.hpp"

using namespace std;

int main(int argc, char* argv[]) {
  Game game; // stack 할당

  game.Initialize();
  game.RunGameLoop();
  game.Destroy();

  return 0;
}
