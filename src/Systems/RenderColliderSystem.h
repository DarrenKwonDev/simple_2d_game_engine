#pragma once

#include "Components/BoxColliderComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"
#include "SDL2/SDL_render.h"

class RenderColliderSystem : public System {
public:
    RenderColliderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    };

    void Update(SDL_Renderer* renderer) {
        for (auto entity : GetSystemEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto collider = entity.GetComponent<BoxColliderComponent>();

            SDL_Rect colliderRect = {static_cast<int>(transform.mPosition.x + collider.mOffset.x),
                                     static_cast<int>(transform.mPosition.y + collider.mOffset.y),
                                     static_cast<int>(collider.mWidth),
                                     static_cast<int>(collider.mHeight)};

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &colliderRect);
        }
    };

private:
};