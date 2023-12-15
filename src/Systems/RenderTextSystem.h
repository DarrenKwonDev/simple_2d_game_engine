#pragma once

#include "AssetStore/AssetStore.h"
#include "Components/TextLabelComponent.h"
#include "ECS/ECS.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_surface.h"
#include <SDL2/SDL.h>
#include <cstddef>
#include <memory>

class RenderTextSystem : public System {
public:
    RenderTextSystem() {
        RequireComponent<TextLabelComponent>();
    }

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
        for (auto entity : GetSystemEntities()) {
            auto& textLabel = entity.GetComponent<TextLabelComponent>();

            SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont(textLabel.mFontAssetId),
                                                          textLabel.mText.c_str(),
                                                          textLabel.mColor);

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int w, h;
            SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

            SDL_Rect destRect = {static_cast<int>(textLabel.mPosition.x) - (textLabel.mIsFixed ? 0 : camera.x),
                                 static_cast<int>(textLabel.mPosition.y) - (textLabel.mIsFixed ? 0 : camera.y),
                                 w,
                                 h};

            SDL_RenderCopy(renderer, texture, NULL, &destRect);
        }
    }
};