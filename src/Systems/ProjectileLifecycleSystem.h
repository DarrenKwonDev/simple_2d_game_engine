#pragma once

#include "Components/ProjectileComponent.h"
#include "ECS/ECS.h"

class ProjectileLifecycleSystem : public System {
private:
public:
    ProjectileLifecycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update() {
        for (Entity entity : GetSystemEntities()) {
            auto& projectile = entity.GetComponent<ProjectileComponent>();

            if (SDL_GetTicks() - projectile.mStartTimeMS > projectile.mDurationMS) {
                entity.Kill();
            }
        }
    }
};