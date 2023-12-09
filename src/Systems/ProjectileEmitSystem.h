#pragma once

#include "Components/BoxColliderComponent.h"
#include "Components/CameraFollowComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"
#include "EventBus/EventBus.h"
#include "Events/KeyPressedEvent.h"
#include "Macros.h"
#include "SDL2/SDL_timer.h"

class ProjectileEmitSystem : public System {
private:
public:
    ProjectileEmitSystem() {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }
    virtual ~ProjectileEmitSystem() = default;

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent& event) {
        if (event.mSymbol == SDLK_SPACE) {
            for (Entity entity : GetSystemEntities()) {
                if (entity.HasComponent<CameraFollowComponent>()) {
                    const auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                    const auto& transform = entity.GetComponent<TransformComponent>();
                    const auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

                    // projectile의 시작 position을 중앙으로 설정.
                    glm::vec2 projectilePosition = transform.mPosition;
                    if (entity.HasComponent<SpriteComponent>()) {
                        const auto& sprite = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += (transform.mScale.x * sprite.mWidth / 2);
                        projectilePosition.y += (transform.mScale.y * sprite.mHeight / 2);
                    }

                    // keyboard control system에 의해 rigid body의 velocity가 변환하면,
                    // 해당 velocity를 기반으로 projectile의 속력과 방향을 결정한다.
                    glm::vec2 projectileVelocity = projectileEmitter.mProjectileVelocity;
                    int directionX = 0;
                    int directionY = 0;
                    if (rigidBody.mVelocity.x > 0) {
                        directionX = +1;
                    }
                    if (rigidBody.mVelocity.x < 0) {
                        directionX = -1;
                    }
                    if (rigidBody.mVelocity.y > 0) {
                        directionY = +1;
                    }
                    if (rigidBody.mVelocity.y < 0) {
                        directionY = -1;
                    }

                    projectileVelocity.x = projectileEmitter.mProjectileVelocity.x * directionX;
                    projectileVelocity.y = projectileEmitter.mProjectileVelocity.y * directionY;

                    Entity projectile = entity.mRegistry->CreateEntity();
                    projectile.Group("projectiles");
                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                    projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
                    projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, ZIndex::Projectile);
                    projectile.AddComponent<BoxColliderComponent>(4, 4, glm::vec2(0));
                    projectile.AddComponent<ProjectileComponent>(projectileEmitter.mIsFriendly,
                                                                 projectileEmitter.mHitPercentDamage,
                                                                 projectileEmitter.mProjectileDurationMS);
                }
            }
        }
    }

    void Update(std::unique_ptr<Registry>& registry) {
        for (Entity entity : GetSystemEntities()) {
            auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
            const auto& transform = entity.GetComponent<TransformComponent>();

            if (projectileEmitter.mRepeatFrequencyMS == DO_NOT_AUTO_RE_EMIT_PROJECTILE_FREQ) {
                continue;
            }

            // auto emit.
            if (SDL_GetTicks() - projectileEmitter.mLastProjectileTime > projectileEmitter.mRepeatFrequencyMS) {

                glm::vec2 projectilePosition = transform.mPosition;
                if (entity.HasComponent<SpriteComponent>()) {
                    const auto sprite = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += transform.mScale.x * sprite.mWidth / 2;
                    projectilePosition.y += transform.mScale.y * sprite.mHeight / 2;
                }

                Entity projectile = registry->CreateEntity();
                projectile.Group("projectiles");
                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                projectile.AddComponent<RigidBodyComponent>(projectileEmitter.mProjectileVelocity);
                projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, ZIndex::Projectile);
                projectile.AddComponent<BoxColliderComponent>(4, 4, glm::vec2(0));
                projectile.AddComponent<ProjectileComponent>(projectileEmitter.mIsFriendly,
                                                             projectileEmitter.mHitPercentDamage,
                                                             projectileEmitter.mProjectileDurationMS);

                projectileEmitter.mLastProjectileTime = SDL_GetTicks();
            }
        }
    }
};