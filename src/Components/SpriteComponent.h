#pragma once

struct SpriteComponent {
    int mWidth;
    int mHeight;

    SpriteComponent(int width = 0, int height = 0) {
        this->mWidth = width;
        this->mHeight = height;
    }
};