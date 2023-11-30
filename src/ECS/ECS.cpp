#include "ECS.h"

Component::Component() {
}

Component::~Component() {
}

Entity::Entity(int id) : mId(id) {
}

Entity::~Entity() {
}

int Entity::GetId() const {
    return this->mId;
}

Registry::Registry() {
}

Registry::~Registry() {
}

void System::AddEntityToSystem(Entity entity) {
}

void System::RemoveEntityFromSystem(Entity entity) {
}

std::vector<Entity> System::GetSystemEntities() const {
    return std::vector<Entity>();
}

const Signature& System::GetComponentSignature() const {
    // TODO: insert return statement here
}
