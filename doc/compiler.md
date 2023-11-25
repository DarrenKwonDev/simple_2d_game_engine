## 컴파일 플래그와 링커 플래그의 구분.

-   컴파일 플래그
    -   `-Wall`: 일반적인 경고 메시지를 모두 활성화합니다.
    -   `-Wextra`: -Wall보다 더 많은 경고 메시지를 활성화합니다.
    -   `-Werror`: 모든 경고를 오류로 처리하여 경고가 발생하면 컴파일을 중단합니다.
    -   `-std=c++17`: C++17 표준을 사용합니다.
    -   `-O1`, `-O2`, `-O3`: 점진적으로 강화되는 최적화 수준을 제공합니다.
    -   `-D` : 매크로 정의 (-DDEBUG, -D_THREAD_SAFE)
    -   `-I` : 추가적으로 정의한 사용자 헤더 파일 경로
-   링커 플래그
    -   `-l`: 라이브러리 이름
    -   `-L`: 라이브러리 경로
    -   `-o`: 출력 파일 이름

```bash
sdl2-config --libs --cflags
-L/opt/homebrew/lib -lSDL2
-I/opt/homebrew/include/SDL2 -D_THREAD_SAFE

sdl2-config --libs
-L/opt/homebrew/lib -lSDL2

# 그러나 막상 찾는 곳은? (실제 경로)
/opt/homebrew/Cellar/sdl2/2.28.5/include/SDL2
```

## 설치한 라이브러리의 경로 문제로 컴파일러와 IDE intellesense의 경로가 일치하지 않을 경우

심볼릭 링크와 실제 경로: Homebrew는 종종 실제 라이브러리 파일을 Cellar 디렉토리에 설치하고, /opt/homebrew/include나 /opt/homebrew/lib와 같은 일반적인 위치에는 심볼릭 링크를 생성합니다. sdl2-config 스크립트는 이러한 심볼릭 링크를 따라가서 헤더 파일과 라이브러리 파일의 위치를 제공합니다.

컴파일러는 sdl2-config 스크립트의 결과를 기반으로 SDL2 헤더 파일과 라이브러리를 찾습니다. 반면, IDE(Integrated Development Environment)는 프로젝트 설정에 명시된 경로를 따라 헤더 파일을 찾습니다. 이 두 설정이 불일치할 경우, 경로 문제가 발생할 수 있습니다.

가급적이면 c_cpp_properties.json에 Cellar가 아닌 /opt/homebrew/include, /opt/homebrew/lib를 직접 명시하고, 컴파일러에서도 이를 참조하도록 설정하는 것이 좋다.
