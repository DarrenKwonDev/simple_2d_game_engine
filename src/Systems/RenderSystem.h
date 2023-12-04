#pragma once

#include "../AssetStore/AssetStore.h"
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

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {

        for (Entity entity : GetSystemEntities()) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& sprite = entity.GetComponent<SpriteComponent>();

            SDL_Rect srcRect = sprite.mSrcRect;

            SDL_Rect dstRect = {
                static_cast<int>(transform.mPosition.x),
                static_cast<int>(transform.mPosition.y),
                static_cast<int>(sprite.mWidth * transform.mScale.x),
                static_cast<int>(sprite.mHeight * transform.mScale.y)};

            SDL_RenderCopyEx(
                renderer,
                assetStore->GetTexture(sprite.mAssetId),
                &srcRect,
                &dstRect,
                transform.mRotation,
                NULL,
                SDL_FLIP_NONE);
        };
    };
};