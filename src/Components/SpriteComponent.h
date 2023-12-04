#pragma once

#include <SDL2/SDL.h>
#include <string>

struct SpriteComponent {
    std::string mAssetId;
    int mWidth;
    int mHeight;
    SDL_Rect mSrcRect;

    SpriteComponent(
        std::string assetId = "",
        int width = 0, int height = 0,
        int srcRectX = 0, int srcRectY = 0) {
        this->mAssetId = assetId;
        this->mWidth = width;
        this->mHeight = height;
        this->mSrcRect = {srcRectX, srcRectY, width, height};
    }
};