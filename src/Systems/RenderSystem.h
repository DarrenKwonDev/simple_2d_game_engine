#pragma once

#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"

#include <SDL2/SDL.h>

class RenderSystem : public System {
private:
public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    };

    void Update(SDL_Renderer* renderer) {

        for (Entity entity : GetSystemEntities()) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& sprite = entity.GetComponent<SpriteComponent>();

            SDL_Rect objRect = {
                static_cast<int>(transform.mPosition.x),
                static_cast<int>(transform.mPosition.y),
                sprite.mWidth,
                sprite.mHeight};

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &objRect);
        };
    };
};