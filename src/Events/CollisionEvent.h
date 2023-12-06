#pragma once

#include "ECS/ECS.h"
#include "EventBus/Event.h"

class CollisionEvent : public Event {
private:
public:
    Entity a;
    Entity b;
    CollisionEvent(Entity a, Entity b) : a(a), b(b){};
};
