#pragma once

#include "AssetStore/AssetStore.h"
#include "Components/HealthComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"
#include <SDL2/SDL.h>

class RenderHealthBarSystem : public System {
public:
    RenderHealthBarSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
        RequireComponent<HealthComponent>();
    }

    void Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
        for (Entity entity : GetSystemEntities()) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& sprite = entity.GetComponent<SpriteComponent>();
            auto& health = entity.GetComponent<HealthComponent>();

            // 체력의 상태에 따라 health bar의 색을 조정
            SDL_Color healthBarColor = {255, 255, 255};

            if (health.mHealthPercentage >= 0 && health.mHealthPercentage < 40) {
                healthBarColor = {255, 0, 0};
            }
            if (health.mHealthPercentage >= 40 && health.mHealthPercentage < 80) {
                healthBarColor = {255, 255, 0};
            }
            if (health.mHealthPercentage >= 80 && health.mHealthPercentage <= 100) {
                healthBarColor = {0, 255, 0};
            }

            // health bar의 크기는 임의 조정
            int healthBarWidth = 32;
            int healthBarHeight = 2;
            double healthBarPosX = (transform.mPosition.x + (sprite.mWidth * transform.mScale.x)) - camera.x;
            double healthBarPosY = (transform.mPosition.y) - camera.y;

            SDL_Rect healthBarRectangle = {static_cast<int>(healthBarPosX),
                                           static_cast<int>(healthBarPosY),
                                           static_cast<int>(healthBarWidth * (health.mHealthPercentage / 100.0)),
                                           static_cast<int>(healthBarHeight)};
            SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
            SDL_RenderFillRect(renderer, &healthBarRectangle);

            std::string healthText = std::to_string(health.mHealthPercentage);
            SDL_Surface* surface =
                TTF_RenderText_Blended(assetStore->GetFont("charriot-font-16"), healthText.c_str(), healthBarColor);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int x = 0;
            int y = 0;
            SDL_QueryTexture(texture, NULL, NULL, &x, &y);
            SDL_Rect healthBarTextRectangle = {static_cast<int>(healthBarPosX),
                                               static_cast<int>(healthBarPosY) + 5,
                                               x,
                                               y};

            SDL_RenderCopy(renderer, texture, NULL, &healthBarTextRectangle);

            SDL_DestroyTexture(texture);
        }
    }
};
