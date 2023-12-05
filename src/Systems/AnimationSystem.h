#pragma once

#include "../Components/AnimationComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"

class AnimationSystem : public System {
public:
    AnimationSystem() {
        RequireComponent<AnimationComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update() {
        for (Entity entity : GetSystemEntities()) {
            // auto animation = entity.GetComponent<AnimationComponent>();
            // auto sprite = entity.GetComponent<SpriteComponent>();

            // TODO: change current frame and src, dst rect
        }
    }

private:
};