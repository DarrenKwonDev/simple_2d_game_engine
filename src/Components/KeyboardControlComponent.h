#pragma once

#include "glm/glm.hpp"

struct KeyboardControlComponent {
    glm::vec2 mUpVelocity;
    glm::vec2 mRightVelocity;
    glm::vec2 mDownVelocity;
    glm::vec2 mLeftVelocity;

    KeyboardControlComponent(glm::vec2 up = glm::vec2(0),
                             glm::vec2 right = glm::vec2(0),
                             glm::vec2 down = glm::vec2(0),
                             glm::vec2 left = glm::vec2(0)) {
        this->mUpVelocity = up;
        this->mRightVelocity = right;
        this->mDownVelocity = down;
        this->mLeftVelocity = left;
    }
};