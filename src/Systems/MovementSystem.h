#pragma once

#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "Components/SpriteComponent.h"
#include "EventBus/EventBus.h"
#include "Events/CollisionEvent.h"
#include "Game/Game.h"
#include "SDL2/SDL_render.h"

class MovementSystem : public System {
private:
    int mMapMargin = 100;

public:
    MovementSystem() {
        // mSysRequiredComponentSignature will be set
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void OnEnemyObstacleCollision(Entity enemy, Entity obstacle) {
        if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
            auto& rigidbody = enemy.GetComponent<RigidBodyComponent>();
            auto& sprite = enemy.GetComponent<SpriteComponent>();

            if (rigidbody.mVelocity.x != 0) {
                rigidbody.mVelocity.x *= -1;
                sprite.mFlip = (sprite.mFlip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
            if (rigidbody.mVelocity.y != 0) {
                rigidbody.mVelocity.y *= -1;
                sprite.mFlip = (sprite.mFlip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
            }
        }
    }

    void OnCollision(CollisionEvent& event) {
        Entity a = event.a;
        Entity b = event.b;

        if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
            OnEnemyObstacleCollision(a, b);
        }
        if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")) {
            OnEnemyObstacleCollision(b, a);
        }
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventManager) {
        eventManager->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
    }

    void Update(double deltaTime) {
        // loop all entities that the system is interested in
        // update entity per frame
        for (Entity entity : GetSystemEntities()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            const auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

            transform.mPosition.x += rigidbody.mVelocity.x * deltaTime;
            transform.mPosition.y += rigidbody.mVelocity.y * deltaTime;

            // screen에서 벗어나는 기준으로 삼으면 안되고, 맵 기준으로 삼아야 함.
            // 스크린 밖에도 맵이 있다.
            bool isEntityOutOfMap =
                transform.mPosition.x < -mMapMargin || transform.mPosition.x > Game::mMapWidth + mMapMargin ||
                transform.mPosition.y < -mMapMargin || transform.mPosition.y > Game::mMapHeight + mMapMargin;

            if (isEntityOutOfMap && !entity.HasTag("player")) {
                entity.Kill();
            }
        }
    }
};