#pragma once

#include <SDL2/SDL.h>
#include <cstdint>

struct AnimationComponent {
    int mNumFrames;      // animation이 몇 개의 프레임(이미지)로 구성될 것인가
    int mFrameSpeedRate; // 애니메이션의 속도 (frame per sec)
    bool mIsLoop;        // animation이 반복되어야 하는가?
    int mCurrentFrame;   // loop 도중 몇 번째 프레임을 렌더하고 있는가를 추적하기 위한 변수
    uint32_t mStartTime;

    AnimationComponent(int numFrames = 1, int frameSpeedRate = 1, bool isLoop = true) {
        this->mNumFrames = numFrames;
        this->mCurrentFrame = 1;
        this->mFrameSpeedRate = frameSpeedRate;
        this->mIsLoop = isLoop;
        this->mStartTime = SDL_GetTicks();
    }
};