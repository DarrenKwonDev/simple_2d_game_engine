#include <algorithm>

#include "../Logger/Logger.h"
#include "ECS.h"

// static 멤버 변수 초기화
int IComponent::nextId = 0;

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
    Logger::Log("create registry");
}

Registry::~Registry() {
    Logger::Log("deconstruct registry");
}

void Registry::Update() {
    for (Entity entity : mEntitiesToBeAdded) {
        Registry::AddEntityToSystems(entity);
    }
    mEntitiesToBeAdded.clear();

    // for (auto entity : mEntitiesToBeKilled)
    // {
    //     Registry::
    // }
    // mEntitiesToBeKilled.clear();
}

Entity Registry::CreateEntity() {
    int entityId = mNumEntities++;

    if (entityId >= mEntityComponentSignatures.size()) {
        // 비록 vector를 사용했지만, 인덱스 기반 접근의 안전성을 위해서 수동 resize.
        // (vector[idx]) 꼴의 접근은 vector의 cap을 증가시키지 않음.
        mEntityComponentSignatures.resize(entityId + 1);
    }

    Entity entity(entityId);

    mEntitiesToBeAdded.insert(entity);

    Logger::Log("Entity created, id: " + std::to_string(entityId));

    return Entity(3);
}

void Registry::AddEntityToSystems(Entity entity) {
    const int entityId = entity.GetId();
    const Signature& entityComponentSignature = mEntityComponentSignatures[entityId];

    for (auto& [_, system] : mSystemsMap) {
        const Signature& systemComponentSignature = system->GetComponentSignature();

        // 참조의 비교는 값 비교임.
        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system->AddEntityToSystem(entity);
        }
    }
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
