#pragma once

#include "SDL2/SDL_keycode.h"

#include "Components/KeyboardControlComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "ECS/ECS.h"
#include "EventBus/EventBus.h"
#include "Events/KeyPressedEvent.h"
#include <memory>

class KeyboardControlSystem : public System {
private:
public:
    KeyboardControlSystem() {
        RequireComponent<KeyboardControlComponent>();
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void OnKeyPressed(KeyPressedEvent& event) {
        // std::string keyCode = std::to_string(event.mSymbol);
        // std::string keySymbol(1, event.mSymbol);
        // Logger::Log("key pressed event emit: " + keyCode + " " + keySymbol);

        for (Entity entity : GetSystemEntities()) {
            const KeyboardControlComponent& keyboardControl = entity.GetComponent<KeyboardControlComponent>();
            SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
            RigidBodyComponent& rigidBody = entity.GetComponent<RigidBodyComponent>();

            switch (event.mSymbol) {
            case SDLK_UP:
                rigidBody.mVelocity = keyboardControl.mUpVelocity;
                sprite.mSrcRect.y = sprite.mHeight * 0; // png img의 첫번째 row
                break;
            case SDLK_RIGHT:
                rigidBody.mVelocity = keyboardControl.mRightVelocity;
                sprite.mSrcRect.y = sprite.mHeight * 1; // png img의 두번째 row
                break;
            case SDLK_DOWN:
                rigidBody.mVelocity = keyboardControl.mDownVelocity;
                sprite.mSrcRect.y = sprite.mHeight * 2; // png img의 세번째 row
                break;
            case SDLK_LEFT:
                rigidBody.mVelocity = keyboardControl.mLeftVelocity;
                sprite.mSrcRect.y = sprite.mHeight * 3; // png img의 네번째 row
                break;
            default:
                // nil
                break;
            }
        }
    }
    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }
    void Update() {
    }
};