#pragma once

#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"

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

            // Logger::Log("Entity id = " +
            //             std::to_string(entity.GetId()) +
            //             " position is update to (" +
            //             std::to_string(transform.mPosition.x) +
            //             ", " +
            //             std::to_string(transform.mPosition.y) +
            //             ")");
        }
    }
};