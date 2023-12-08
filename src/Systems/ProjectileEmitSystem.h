#pragma once

#include "Components/BoxColliderComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"
#include "SDL2/SDL_timer.h"
#include <memory>

class ProjectileEmitSystem : public System {
private:
public:
    ProjectileEmitSystem() {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }
    virtual ~ProjectileEmitSystem() = default;

    void Update(std::unique_ptr<Registry>& registry) {
        for (Entity entity : GetSystemEntities()) {
            auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
            const auto& transform = entity.GetComponent<TransformComponent>();

            if (SDL_GetTicks() - projectileEmitter.mLastProjectileTime > projectileEmitter.mRepeatFrequencyMS) {

                glm::vec2 projectilePosition = transform.mPosition;
                if (entity.HasComponent<SpriteComponent>()) {
                    const auto sprite = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += transform.mScale.x * sprite.mWidth / 2;
                    projectilePosition.y += transform.mScale.y * sprite.mHeight / 2;
                }

                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                projectile.AddComponent<RigidBodyComponent>(projectileEmitter.mProjectileVelocity);
                projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 10);
                projectile.AddComponent<BoxColliderComponent>(4, 4, glm::vec2(0));
                projectile.AddComponent<ProjectileComponent>(projectileEmitter.mIsFriendly,
                                                             projectileEmitter.mHitPercentDamage,
                                                             projectileEmitter.mProjectileDurationMS);

                projectileEmitter.mLastProjectileTime = SDL_GetTicks();
            }
        }
    }
};