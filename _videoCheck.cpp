#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL 초기화 실패: " << SDL_GetError() << std::endl;
    return 1;
  }

  int displayCount = SDL_GetNumVideoDisplays();
  std::cout << "연결된 모니터 수: " << displayCount << std::endl;

  for (int i = 0; i < displayCount; i++) {
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(i, &mode);
    const char* displayName = SDL_GetDisplayName(i);

    std::cout << "모니터 " << displayName << ": " << std::endl;
    std::cout << "해상도 " << mode.w << "x" << mode.h << ", ";
    std::cout << "refresh rate " << mode.refresh_rate << "Hz" << std::endl;
  }

  SDL_Quit();
  return 0;
}
