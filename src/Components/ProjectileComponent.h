#pragma once

#include "SDL2/SDL_timer.h"
#include <cstdint>

struct ProjectileComponent {
    bool mIsFriendly;
    int mHitPercentDamage;
    int mDurationMS;
    uint32_t mStartTimeMS;

    ProjectileComponent(bool isFriendly = false, int hitPercentDamage = 0, int durationMS = 0) {
        this->mIsFriendly = isFriendly;
        this->mHitPercentDamage = hitPercentDamage;
        this->mDurationMS = durationMS;
        this->mStartTimeMS = SDL_GetTicks();
    }
};