#pragma once

#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "ECS/ECS.h"
#include "EventBus/EventBus.h"
#include "Events/KeyPressedEvent.h"
#include <memory>
#include <string>
class KeyboardControlSystem : public System {
private:
public:
    KeyboardControlSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void OnKeyPressed(KeyPressedEvent& event) {
        std::string keyCode = std::to_string(event.mSymbol);
        std::string keySymbol(1, event.mSymbol);
        Logger::Log("key pressed event emit: " + keyCode + " " + keySymbol);
    }
    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }
    void Update() {
    }
};