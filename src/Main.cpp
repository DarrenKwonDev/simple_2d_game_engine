#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    cout << "Hello, world!" << endl;
    SDL_Quit();
    return 0;
}
