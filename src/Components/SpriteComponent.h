#pragma once

#include <string>

struct SpriteComponent {
    std::string mAssetId;
    int mWidth;
    int mHeight;

    SpriteComponent(std::string assetId = "", int width = 0, int height = 0) {
        this->mAssetId = assetId;
        this->mWidth = width;
        this->mHeight = height;
    }
};