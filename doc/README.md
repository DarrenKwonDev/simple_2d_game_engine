# 2d-game-engine-poc

lectured by [pikuma game engine](https://github.com/gustavopezzi/game-engine-cpp-2d/tree/main) lecture

## deps

```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer lua

# sd2 check
sdl2-config --version # 2.28.5

sdl2-config --libs
# 링커 플래그 제공
# 링커가 SDL2 라이브러리를 찾고 링크하는 데 필요한 설정을 제공합니다

sdl2-config --cflags
# 컴파일러 플래그 제공
# SDL2 헤더 파일의 경로를 출력.
# 컴파일러가 SDL2의 헤더 파일을 찾을 수 있도록 해줍니다.

sdl2-config --libs --cflags # 동시에 출력

brew info sdl2 # check include path

# 현재 맥 환경에선 --cflags가 -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE 를 출력함.
# 문제는 이렇나 방식이면 #include <SDL.h> 방식으로 include해야 함.
clang++ -Wall -std=c++17 Main.cpp $(sdl2-config --libs --cflags) -o Main

# 코딩 컨벤션에 맞게 <SDL2/SDL.h> 로 사용하기 위해 -I 옵션을 수정하여 아래처럼 적용함.
clang++ -Wall -std=c++17 Main.cpp -L/opt/homebrew/lib -lSDL2 -I/opt/homebrew/include -D_THREAD_SAFE -o Main

```

### library를 pre-compiled binary로 사용하기 vs 내장 lib로 사용하기

-   pre-compiled binary로 사용하기

    -   이 프로젝트에선 SDL2, lua는 사용하고 있어 -l 옵션으로 링킹해주고, 바이너리를 -L로, 헤더 경로를 -I로 추가해주면 됨.
    -   이 방식의 장점은, 컴파일 타임을 잡아 먹지 않는다는 것.

-   내장 lib로 사용하기
    -   이 프로젝트에선 glm, imgui, sol은 내장 lib로써 사용하고 있어 소스코드를 전부 다 담고 있다. -I 경로만 추가해주면 됨.
    -   이 방식의 장점은, 커스터마이징이 용이하고 플랫폼에 종속되지 않는다는 것. 물론 손이 좀 더 가긴 한다.
