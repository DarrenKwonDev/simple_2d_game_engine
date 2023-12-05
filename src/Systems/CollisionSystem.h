#pragma once

#include "Components/BoxColliderComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"
#include <string>

class CollisionSystem : public System {
public:
    CollisionSystem() {
        RequireComponent<BoxColliderComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update() {

        // AABB collision check.
        auto entities = GetSystemEntities();
        for (auto it = entities.begin(); it != entities.end(); it++) {
            Entity a = *it;
            auto aTransform = a.GetComponent<TransformComponent>();
            auto aCollider = a.GetComponent<BoxColliderComponent>();

            for (auto jt = std::next(it); jt != entities.end(); jt++) {
                Entity b = *jt;
                auto bTransform = b.GetComponent<TransformComponent>();
                auto bCollider = b.GetComponent<BoxColliderComponent>();

                bool isCollied = CheckAABBCollision(
                    aTransform.mPosition.x + aCollider.mOffset.x, aTransform.mPosition.y + aCollider.mOffset.x,
                    aCollider.mWidth, aCollider.mHeight, bTransform.mPosition.x + bCollider.mOffset.x,
                    bTransform.mPosition.y + bCollider.mOffset.y, bCollider.mWidth, bCollider.mHeight);
                if (isCollied) {
                    Logger::Log("entity " + std::to_string(a.GetId()) + " colliding with entity " +
                                std::to_string(b.GetId()));
                }
            }
        }
    }

    bool CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
        return (aX + aW > bW && aX < bX + bW && aY + aH > bY && aY < bY + bH);
    };

private:
};