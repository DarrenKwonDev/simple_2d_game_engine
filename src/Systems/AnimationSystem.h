#pragma once

#include "../Components/AnimationComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"
#include "SDL2/SDL_timer.h"

class AnimationSystem : public System {
public:
    AnimationSystem() {
        RequireComponent<AnimationComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update() {
        for (Entity entity : GetSystemEntities()) {
            auto& animation = entity.GetComponent<AnimationComponent>();
            auto& sprite = entity.GetComponent<SpriteComponent>();

            animation.mCurrentFrame =
                ((SDL_GetTicks() - animation.mStartTime) * animation.mFrameSpeedRate / 1000) % animation.mNumFrames;

            sprite.mSrcRect.x = animation.mCurrentFrame * sprite.mWidth;
        }
    }

private:
};