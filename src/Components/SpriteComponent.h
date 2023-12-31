#pragma once

#include "Macros.h"
#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <string>

struct SpriteComponent {
    std::string mAssetId;
    int mWidth;
    int mHeight;
    ZIndex mZIndex;
    SDL_RendererFlip mFlip;
    bool mIsFixed;
    SDL_Rect mSrcRect;

    SpriteComponent(std::string assetId = "",
                    int width = 0,
                    int height = 0,
                    ZIndex zIndex = ZIndex::Background,
                    bool isFixed = false,
                    int srcRectX = 0,
                    int srcRectY = 0) {
        this->mAssetId = assetId;
        this->mWidth = width;
        this->mHeight = height;
        this->mZIndex = zIndex;
        this->mFlip = SDL_FLIP_NONE;
        this->mIsFixed = isFixed;
        this->mSrcRect = {srcRectX, srcRectY, width, height};
    }
};