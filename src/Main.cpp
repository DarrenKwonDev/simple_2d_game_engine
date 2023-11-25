#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "sol/sol.hpp"

using namespace std;

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    sol::state lua;
    lua.open_libraries(sol::lib::base);

    glm::vec2 velocity = glm::vec2(5.0, -2.5);
    velocity = glm::normalize(velocity);

    cout << "Hello, world!" << endl;
    SDL_Quit();
    return 0;
}
