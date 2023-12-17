#pragma once

#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "Game/Game.h"

class MovementSystem : public System {
private:
public:
    MovementSystem() {
        // mSysRequiredComponentSignature will be set
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
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
            bool isEntityOutOfMap = transform.mPosition.x < 0 || transform.mPosition.x > Game::mMapWidth ||
                                    transform.mPosition.y < 0 || transform.mPosition.y > Game::mMapHeight;

            if (isEntityOutOfMap && !entity.HasTag("player")) {
                entity.Kill();
            }
        }
    }
};