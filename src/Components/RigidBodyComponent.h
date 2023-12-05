#pragma once

#include "glm/glm.hpp"

struct RigidBodyComponent {
    glm::vec2 mVelocity;

    RigidBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0)) { this->mVelocity = velocity; }
};
