#include "ECS.h"

#include <algorithm>

////////////////////////////////////////////////////////
// entity
////////////////////////////////////////////////////////

Entity::Entity(int id) : mId(id) {
}

Entity::~Entity() {
}

int Entity::GetId() const {
    return this->mId;
}

////////////////////////////////////////////////////////
// registry
////////////////////////////////////////////////////////

Registry::Registry() {
}

Registry::~Registry() {
}

////////////////////////////////////////////////////////
// system
////////////////////////////////////////////////////////

void System::AddEntityToSystem(Entity entity) {
    mEntities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
    // https://en.cppreference.com/w/cpp/algorithm/remove
    // remove_if는 pred 식에 만족하는 원소를 벡터의 끝으로 이동시킨다.
    // 반환하는 것은, 이동된 요소들의 시작 지점이다.
    // 실제로 값이 지워지지는 않기에 erase가 요구된다.
    // remove(if), erase idiom은 직관적이지 않지만 관습이다.
    mEntities.erase(
        std::remove_if(mEntities.begin(), mEntities.end(), [&entity](Entity elem) {
            return entity == elem;
        }),
        mEntities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
    return mEntities;
}

const Signature& System::GetComponentSignature() const {
    return mComponentSignature;
}
