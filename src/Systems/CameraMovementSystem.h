#pragma once

#include "Components/CameraFollowComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"
#include "Game/Game.h"
#include "SDL2/SDL_rect.h"
#include <string>

class CameraMovementSystem : public System {
private:
public:
    CameraMovementSystem() {
        RequireComponent<CameraFollowComponent>();
        RequireComponent<TransformComponent>();
    }
    virtual ~CameraMovementSystem() = default;

    void Update(SDL_Rect& camera) {
        for (Entity entity : GetSystemEntities()) {
            auto& transform = entity.GetComponent<TransformComponent>();

            // 화면 axis의 중앙을 넘어야 카메라가 움직이도록하자.
            if (transform.mPosition.x + static_cast<int>(camera.w / 2) < Game::mMapWidth) {
                camera.x = transform.mPosition.x - static_cast<int>(Game::mWindowWidth / 2);
            }
            if (transform.mPosition.y + static_cast<int>(camera.h / 2) < Game::mMapHeight) {
                camera.y = transform.mPosition.y - static_cast<int>(Game::mWindowHeight / 2);
            }

            // 화면 내로 유지
            camera.x = camera.x < 0 ? 0 : camera.x;
            camera.x = camera.x > camera.w ? camera.w : camera.x;
            camera.y = camera.y < 0 ? 0 : camera.y;
            camera.y = camera.y > camera.h ? camera.h : camera.y;

            // Logger::Log("camera position: " + std::to_string(camera.x) + " " + std::to_string(camera.y));
        }
    }
};