#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <string>

struct TextLabelComponent {
    glm::vec2 mPosition;
    std::string mText;
    std::string mFontAssetId;
    SDL_Color mColor;
    bool mIsFixed;

    TextLabelComponent(glm::vec2 position = glm::vec2(0, 0),
                       const std::string& text = "",
                       const std::string& fontAssetId = "default_font",
                       const SDL_Color& color = {0, 0, 0},
                       bool isFixed = true) {
        this->mPosition = position;
        this->mText = text;
        this->mFontAssetId = fontAssetId;
        this->mColor = color;
        this->mIsFixed = isFixed;
    }
};