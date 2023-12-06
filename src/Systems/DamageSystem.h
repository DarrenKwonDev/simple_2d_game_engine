#pragma once

#include "Components/BoxColliderComponent.h"
#include "ECS/ECS.h"
#include "EventBus/EventBus.h"
#include "Events/CollisionEvent.h"
#include <memory>
class DamageSystem : public System {
private:
public:
    DamageSystem() {
        RequireComponent<BoxColliderComponent>();
    }

    void onCollision(CollisionEvent& event) {
        Logger::Log("damage system got collision event " + std::to_string(event.a.GetId()) + " and " +
                    std::to_string(event.b.GetId()));
        event.a.Kill();
        event.b.Kill();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {

        // 함수 매개변수의 타입을 바탕으로 템플릿 매개변수의 타입을 추론가능.
        // 따라서 모든 typename을 명시할 필요는 없다.
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
    }

    void Update() {
    }
};