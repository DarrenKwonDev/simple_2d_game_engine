

<!-- toc -->

- [2d-game-engine-poc](#2d-game-engine-poc)
  * [configure](#configure)
    + [deps](#deps)
    + [library를 pre-compiled binary로 사용하기 vs 내장 lib로 사용하기](#library%EB%A5%BC-pre-compiled-binary%EB%A1%9C-%EC%82%AC%EC%9A%A9%ED%95%98%EA%B8%B0-vs-%EB%82%B4%EC%9E%A5-lib%EB%A1%9C-%EC%82%AC%EC%9A%A9%ED%95%98%EA%B8%B0)
  * [game things](#game-things)
    + [full screen vs fake full screen(borderless window)](#full-screen-vs-fake-full-screenborderless-window)
    + [rendererFlags](#rendererflags)
    + [VSync (vertical sync, 수직 동기화)](#vsync-vertical-sync-%EC%88%98%EC%A7%81-%EB%8F%99%EA%B8%B0%ED%99%94)

<!-- tocstop -->

# 2d-game-engine-poc

lectured by [pikuma game engine](https://github.com/gustavopezzi/game-engine-cpp-2d/tree/main) lecture

## configure

### deps

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

## game things

### full screen vs fake full screen(borderless window)

찐 full screen은 다른 화면 전환이 번거로움. (전체 화면)
Fake Fullscreen은 사실상 크기를 조정한 창 모드. (전체 창모드)

### rendererFlags

SDL_CreateRenderer에서 GPU 사용이 가능

[SDL_RendererFlags](https://wiki.libsdl.org/SDL2/SDL_RendererFlags) 참고.

-   SDL_RENDERER_SOFTWARE : 하드웨어 가속 사용 안하고 CPU 사용
-   SDL_RENDERER_ACCELERATED : dedicated graphics card (GPU) 사용
-   SDL_RENDERER_PRESENTVSYNC : VSync 사용. tearing 방지
-   SDL_RENDERER_TARGETTEXTURE

```cpp
SDL_CreateRenderer(
    window,
    -1,
    SDL_RENDERER_ACCELERATED
);
```

### VSync (vertical sync, 수직 동기화)

아래와 같은 tearing은 게임이 매 프레임마다 렌더링을 하는 사이 모니터의 refresh rate(주사율, 재생률) 과 맞지 않아서 그리는 도중을 보여줘서 보이는 현상임.

<img src="./imgs/screen_tearing.webp" />

이를 위해 게임 렌더러의 프레젠테이션(화면에 그리는 것)을 모니터의 refresh rate와 동기화하는 기술이 vsync이며 결과적으론 tearing을 방지 한다.

추가로, GPU가 모니터의 refresh rate보다 더 많은 프레임을 렌더링하지 않도록 제한함으로써, 불필요한 GPU 부하를 줄일 수도 있다.

문제는 모니터의 refresh rate가 60Hz라면, 게임의 fps는 60fps로 제한되며 (60fps 이상으로 렌더링해도 60fps로 제한됨) 만약 60fps를 못 맞춘다면 나누어 떨어지는 30fps로 렌더링됨. 결국 GPU가 힘들면 fps가 낮아짐.
