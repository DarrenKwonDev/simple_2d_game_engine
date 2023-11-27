<!-- toc -->

-   [2d-game-engine-poc](#2d-game-engine-poc)
    -   [configure](#configure)
        -   [deps](#deps)
        -   [library를 pre-compiled binary로 사용하기 vs 내장 lib로 사용하기](#library를-pre-compiled-binary로-사용하기-vs-내장-lib로-사용하기)
    -   [game things](#game-things)
        -   [full screen vs fake full screen(borderless window)](#full-screen-vs-fake-full-screenborderless-window)
        -   [rendererFlags](#rendererflags)
        -   [VSync (vertical sync, 수직 동기화)](#vsync-vertical-sync-수직-동기화)
        -   [Double-Buffered Renderer](#double-buffered-renderer)
        -   [Fixed Time Step(Frame Rate Independence) game loop](#fixed-time-stepframe-rate-independence-game-loop)
        -   [frame drop compensate with delta time](#frame-drop-compensate-with-delta-time)
    -   [SLD2](#sld2)
        -   [surface vs texture](#surface-vs-texture)

<!-- tocstop -->

# 2d-game-engine-poc

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

문제는 모니터의 refresh rate가 60Hz라면, 게임의 fps는 60fps로 제한되며 (60fps 이상으로 렌더링해도 60fps 이상이라고 인식하지 못함. 애초에 주사율이 초당 60번 refresh되니까.) 만약 60fps를 못 맞춘다면 나누어 떨어지는 30fps로 렌더링됨. 결국 GPU가 힘들면 fps가 낮아짐.

### Double-Buffered Renderer

back buffer와 front buffer의 이중 운용.  
draw를 back buffer에서 먼저하고 front buffer로 swap하는 방식.  
glitch를 방지하고, 렌더링이 완료되지 않은 프레임은 화면에 보여지지 않는 장점이 있음.

### Fixed Time Step(Frame Rate Independence) game loop

아래 코드는 while loop를 도는데 구동되는 컴퓨터의 CPU 성능에 의존적이다.
즉, 게임을 구동하는 클라이언트마다 동일한 게임 경험을 줄 수 없다는 것을 의미한다.

```cpp
void Game::RunGameLoop() {
    Game::Setup();
    while (mIsRunning) {
        Game::ProcessInput();
        Game::Update();
        Game::Render();
    }
}
```

이 문제에 대한 해결책으로

-   고정 프레임 레이트 업데이트
-   시간 기반 업데이트
-   등... 이 존재한다.

### frame drop compensate with delta time

unity, love2d 등 웬만한 게임 엔진은 game loop를 메서드 형식으로 노출하고 있다. 그 중에서도 update는 frame마다 호출된다.

따라서, fps가 하락하면 update 호출 횟수도 줄어듦. frame이 바뀌어도 동일한 정도로 움직이게 만들어야 함.

즉, 다른 frame rate에서도 속도 등이 frame rate에 영향을 받지 않게 만들어야 한다.

해결 방법의 핵심 : if frame rate drop, should be compensate for it.

-   case 1. 60fps -> 30fps
    update 호출이 덜 일어나지만 dt가 늘어났으므로 dt를 곱하여 compensate.

-   case 2. 60fps -> 120fps
    update 호출이 더 자주 일어나지만 dt가 줄어들었으므로 dt를 곱하면 compensate됨.

-   dt를 곱함으로써 frame rate에 영향을 받지 않고, '초' 단위로 속도를 조절할 수 있게 됨.

    ```lua
    -- 17pixel per second
    player.x = player.x + 17 * dt
    ```

## SLD2

### surface vs texture

SDL_Surface는 이미지 처리나 간단한 소프트웨어 렌더링에 적합하고, SDL_Texture는 GPU를 사용한 빠른 화면 렌더링에 적합합니다.

일반적으로 이미지 파일을 로드하고 조작할 때는 SDL_Surface를 사용하고, 화면에 이미지를 렌더링할 때는 SDL_Texture로 변환하여 사용합니다.

SDL_Surface는 시스템 메모리에 저장되고, SDL_Texture는 GPU의 비디오 메모리에 저장됩니다.

```

```
