#pragma once

#include "Components/CameraFollowComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"
#include "SDL2/SDL_rect.h"

class CameraMovementSystem : public System {
private:
public:
    CameraMovementSystem() {
        RequireComponent<CameraFollowComponent>();
        RequireComponent<TransformComponent>();
    }
    virtual ~CameraMovementSystem() = default;

    void Update(SDL_Rect& camera) {
        for (Entity entity : GetSystemEntities()) {
            auto& transform = entity.GetComponent<TransformComponent>();

            camera.x = transform.mPosition.x;
            camera.y = transform.mPosition.y;

            Logger::Log("camera x, y changed");
        }
    }
};