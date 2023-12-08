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
#include "Events/CollisionEvent.h"
#include "Events/KeyPressedEvent.h"
#include "SDL2/SDL_timer.h"
#include "glm/fwd.hpp"
#include <memory>

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

                    glm::vec2 projectilePosition = transform.mPosition;
                    if (entity.HasComponent<SpriteComponent>()) {
                        const auto& sprite = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += transform.mScale.x * sprite.mWidth / 2;
                        projectilePosition.y += transform.mScale.y * sprite.mHeight / 2;
                    }

                    glm::vec2 projectileVelocity = projectileEmitter.mProjectileVelocity;
                    int directionX = 0;
                    int directionY = 0;
                    if (rigidBody.mVelocity.x > 0) {
                        directionX = 1;
                    } else if (rigidBody.mVelocity.x < 0) {
                        directionX = -1;
                    }
                    if (rigidBody.mVelocity.y > 0) {
                        directionY = 1;
                    } else if (rigidBody.mVelocity.y < 0) {
                        directionY = -1;
                    }

                    projectileVelocity.x += projectileEmitter.mProjectileVelocity.x * directionX;
                    projectileVelocity.y += projectileEmitter.mProjectileVelocity.y * directionY;

                    Entity projectile = entity.mRegistry->CreateEntity();
                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                    projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
                    projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
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

            // FIXME: chopper의 경우, 0으로 지정하여 발사를 막게끔 처리함.
            // enum으로 만들어서 오해를 하지 않게끔 하는게 좋지 않을까
            if (projectileEmitter.mRepeatFrequencyMS == 0) {
                continue;
            }

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