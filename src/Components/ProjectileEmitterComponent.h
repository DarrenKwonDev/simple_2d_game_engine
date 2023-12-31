#pragma once

#include "SDL2/SDL_timer.h"
#include "glm/glm.hpp"

struct ProjectileEmitterComponent {
    glm::vec2 mProjectileVelocity;
    int mRepeatFrequencyMS;
    int mProjectileDurationMS;
    int mHitPercentDamage;
    bool mIsFriendly;
    uint32_t mLastProjectileTime;

    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0),
                               int repeatFrequencyMS = 0,
                               int projectileDurationMS = 10000,
                               int hitPercentDamage = 10,
                               bool isFriendly = false) {
        this->mProjectileVelocity = projectileVelocity;
        this->mRepeatFrequencyMS = repeatFrequencyMS;
        this->mProjectileDurationMS = projectileDurationMS;
        this->mHitPercentDamage = hitPercentDamage;
        this->mIsFriendly = isFriendly;
        this->mLastProjectileTime = SDL_GetTicks();
    }
};