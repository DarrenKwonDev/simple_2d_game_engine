#include <algorithm>
#include <stdexcept>

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

void Entity::Kill() {
    mRegistry->KillEntity(*this);
}

void Entity::Tag(const TagName& tag) {
    mRegistry->TagEntity(*this, tag);
}
bool Entity::HasTag(const TagName& tag) const {
    return mRegistry->EntityHasTag(*this, tag);
}
void Entity::Group(const GroupName& group) {
    mRegistry->GroupEntity(*this, group);
}
bool Entity::BelongsToGroup(const GroupName& group) const {
    return mRegistry->EntityBelongsToGroup(*this, group);
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

// temp set에 대기 중인 entity를 system에 등록한다.
void Registry::Update() {
    for (Entity entity : mEntitiesToBeAdded) {
        Registry::AddEntityToSystems(entity);
    }
    mEntitiesToBeAdded.clear();

    for (auto entity : mEntitiesToBeKilled) {
        Registry::RemoveEntityFromSystems(entity);

        mEntityComponentSignatures[entity.GetId()].reset();

        // remove entity from component pool
        for (auto pool : mComponentPools) {
            if (pool) {
                pool->RemoveEntity(entity.GetId());
            }
        }

        // for reusing entity
        mFreeIds.push_back(entity.GetId());

        Registry::RemoveEntityTag(entity);
        Registry::RemoveEntityFromGroup(entity);
    }
    mEntitiesToBeKilled.clear();
}

Entity Registry::CreateEntity() {
    int entityId;

    if (mFreeIds.empty()) {
        entityId = mNumEntities++;
        if (entityId >= mEntityComponentSignatures.size()) {
            // 비록 vector를 사용했지만, 인덱스 기반 접근의 안전성을 위해서 수동 resize.
            // (vector[idx]) 꼴의 접근은 vector의 cap을 증가시키지 않음.
            mEntityComponentSignatures.resize(entityId + 1);
        }
    } else {
        entityId = mFreeIds.front();
        mFreeIds.pop_front();
    }

    Entity entity(entityId);
    entity.mRegistry = this;

    mEntitiesToBeAdded.insert(entity);

    Logger::Log("Entity created, id: " + std::to_string(entityId));

    return entity;
}

void Registry::KillEntity(Entity entity) {
    mEntitiesToBeKilled.insert(entity);
}

void Registry::TagEntity(Entity entity, const TagName& tag) {
    mEntityPerTagMap.emplace(tag, entity);
    mTagPerEntityMap.emplace(entity.GetId(), tag);
}
bool Registry::EntityHasTag(Entity entity, const TagName& tag) const {
    auto entityIter = mTagPerEntityMap.find(entity.GetId());
    if (entityIter == mTagPerEntityMap.end()) {
        return false;
    }

    auto tagIter = mEntityPerTagMap.find(tag);
    if (tagIter == mEntityPerTagMap.end()) {
        return false;
    }

    return tagIter->second == entity;
}

Entity Registry::GetEntityByTag(const TagName& tag) const {
    return mEntityPerTagMap.at(tag);
}
void Registry::RemoveEntityTag(Entity entity) {
    auto taggedEntityIter = mTagPerEntityMap.find(entity.GetId());
    if (taggedEntityIter == mTagPerEntityMap.end()) {
        return;
    }
    TagName tag = taggedEntityIter->second;
    mEntityPerTagMap.erase(tag);
    mTagPerEntityMap.erase(taggedEntityIter);
}

void Registry::GroupEntity(Entity entity, const GroupName& group) {
    mEntityPerGroupMap.emplace(group, std::set<Entity>());
    mEntityPerGroupMap.at(group).insert(entity);
    mGroupPerEntityMap.emplace(entity.GetId(), group);
}
bool Registry::EntityBelongsToGroup(Entity entity, const GroupName& group) const {
    // 게임이 처음 시작될 때 group이 생기기도 전에 entity가 부딪히는 경우가 있음.
    // 이 경우를 handling해주지 않으면 crash 발생.
    if (mEntityPerGroupMap.find(group) == mEntityPerGroupMap.end()) {
        return false;
    }
    auto groupEntities = mEntityPerGroupMap.at(group);
    return groupEntities.find(entity) != groupEntities.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const GroupName& group) const {
    auto& groupEntities = mEntityPerGroupMap.at(group);
    return std::vector<Entity>(groupEntities.begin(), groupEntities.end());
}
void Registry::RemoveEntityFromGroup(Entity entity) {
    // 두 가지 맵에서 모두 드랍해야 한다.
    // mGroupPerEntityMap -> entity ID 가진 필드 드랍
    // mEntityPerGroupMap -> entity가 가진 group에서 entity 드랍 (group을 모두 순회하지
    // 않도록하자.)

    auto groupNameIter = mGroupPerEntityMap.find(entity.GetId());
    if (groupNameIter == mGroupPerEntityMap.end()) {
        return;
    }

    // entity가 가진 group에서 entity 드랍
    GroupName group = groupNameIter->second;
    auto& setOfEntities = mEntityPerGroupMap.at(group);
    setOfEntities.erase(entity);

    // 해당 entity에서 group 드랍
    mGroupPerEntityMap.erase(groupNameIter);
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

void Registry::RemoveEntityFromSystems(Entity entity) {
    for (auto& [_, system] : mSystemsMap) {
        system->RemoveEntityFromSystem(entity);
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
    mEntities.erase(std::remove_if(mEntities.begin(),
                                   mEntities.end(),
                                   [&entity](Entity elem) { return entity == elem; }),
                    mEntities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
    return mEntities;
}

const Signature& System::GetComponentSignature() const {
    return mSysRequiredComponentSignature;
}
