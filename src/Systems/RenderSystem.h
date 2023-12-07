#pragma once

#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>

#include "../AssetStore/AssetStore.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"

class RenderSystem : public System {
private:
public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    };

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera) {
        struct RenderableEntity {
            TransformComponent transformComp;
            SpriteComponent spriteComp;
        };
        std::vector<RenderableEntity> RenderableEntityVec;
        for (Entity entity : GetSystemEntities()) {
            RenderableEntity re;
            re.transformComp = entity.GetComponent<TransformComponent>();
            re.spriteComp = entity.GetComponent<SpriteComponent>();
            RenderableEntityVec.emplace_back(re);
        };

        // FIXME: THIS IS BOTTLENECK OF ENGINE
        // zindex 기반  sorting을 update에서 처리하는 것은 일시적인 구현으로 수정이 필요.
        // don't just sort by ZIndex.
        // why? update called per frame.
        // if entity needs to be update, killed, created very frequently,
        // game will be slowed and mess up.
        std::sort(RenderableEntityVec.begin(),
                  RenderableEntityVec.end(),
                  [](const RenderableEntity& a, const RenderableEntity& b) {
                      return a.spriteComp.mZIndex < b.spriteComp.mZIndex;
                  });

        // render concerned entity
        for (RenderableEntity renderableEntity : RenderableEntityVec) {
            const auto& transform = renderableEntity.transformComp;
            const auto& sprite = renderableEntity.spriteComp;

            SDL_Rect srcRect = sprite.mSrcRect;

            // if camera rect move, all entity should be move too!
            SDL_Rect dstRect = {static_cast<int>(transform.mPosition.x - camera.x),
                                static_cast<int>(transform.mPosition.y - camera.y),
                                static_cast<int>(sprite.mWidth * transform.mScale.x),
                                static_cast<int>(sprite.mHeight * transform.mScale.y)};

            SDL_RenderCopyEx(renderer,
                             assetStore->GetTexture(sprite.mAssetId),
                             &srcRect,
                             &dstRect,
                             transform.mRotation,
                             NULL,
                             SDL_FLIP_NONE);
        };
    };
};