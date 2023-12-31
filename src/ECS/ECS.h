#pragma once

#include <bitset>
#include <deque>
#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

// keep track of which components an entity has
// also, which entities a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

typedef int EntityId;
typedef std::string TagName;
typedef std::string GroupName;

////////////////////////////////////////////////////////
// component. pure data
// template class이므로 컴파일 시점에 여러 개의 클래스로 분화된다는 것을 의미.
////////////////////////////////////////////////////////

struct IComponent {
protected:
    static int nextId;
};

template <typename T>
class Component : public IComponent {
public:
    Component();
    virtual ~Component();

    // return unique id of component
    // T가 다를 때 마다 새로운 Component class가 될 것이다.
    // Component<A>에 부여되는 static 변수와 Component<B> 클래스에 부여되는 static 변수는
    // 다르다.
    static int GetId() {
        static auto mId = nextId++;
        return mId;
    }
};

////////////////////////////////////////////////////////
// Entity. just id.
// entity 객체는 component를 소유하지 않고 있다 (entity-component 관계는 모두 pool에서
// 관리됨.)
////////////////////////////////////////////////////////
class Entity {
private:
    EntityId mId;

public:
    Entity(int mId);
    virtual ~Entity();

    Entity(const Entity& rhs) = default;        // 복사 생성
    Entity& operator=(const Entity&) = default; // 복사 대입 연산

    int GetId() const;
    void Kill();

    // 후방에 선언된 Registry를 전방선언하지 않고도 'class' 선언만으로도 대체 가능.
    // circular deps 문제를 일으킬 가능성이 있으니 조심하자.
    class Registry* mRegistry;

    /////////////////////////
    // group tag
    /////////////////////////
    void Tag(const TagName& tag);
    bool HasTag(const TagName& tag) const;
    void Group(const GroupName& group);
    bool BelongsToGroup(const GroupName& group) const;

    /////////////////////////
    // component
    /////////////////////////
    // args에 무엇을 정의하는가에 따라 TArgs가 컴파일타임에 결정된다.
    template <typename TComponent, typename... TArgs>
    void AddComponent(TArgs&&... args);

    template <typename TComponent>
    void RemoveComponent();

    template <typename TComponent>
    bool HasComponent() const;

    template <typename TComponent>
    TComponent& GetComponent() const;

    /////////////////////////
    // operator
    /////////////////////////
    bool operator==(const Entity& rhs) const {
        return mId == rhs.mId;
    };
    bool operator!=(const Entity& rhs) const {
        return mId != rhs.mId;
    };
    bool operator>(const Entity& rhs) const {
        return mId > rhs.mId;
    };
    bool operator<(const Entity& rhs) const {
        return mId < rhs.mId;
    };
};

////////////////////////////////////////////////////////
// system
// 특정 컴포넌트를 가진 엔티티들을 처리
////////////////////////////////////////////////////////
class System {
private:
    // 시스템이 작업을 수행하는데 필요한 컴포넌트를 정의함.
    // 예를 들어, render system은 position comp, graphic comp가 필요함.
    Signature mSysRequiredComponentSignature;

    std::vector<Entity> mEntities;

public:
    System() = default;
    virtual ~System() = default;

    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystem(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature& GetComponentSignature() const;

    template <typename TComponent>
    void RequireComponent();
};

template <typename TComponent>
inline void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    mSysRequiredComponentSignature.set(componentId);
};

////////////////////////////////////////////////////////
// Pool
// Pool is a vector of object of type T.
// use vector for memory contiguity
// vector의 wrapper기 때문에 가급적 vector의 기본 메서드를 변화시키지 않는 것이 좋다.
////////////////////////////////////////////////////////

// IPool을 사용함으로써 Pool 객체들을 그들의 구체적인 유형(T)에 관계없이 다룰 수 있게 됨.
// 이 경우엔 template class를 또다른 곳에서 template으로 받아야 하는 상황이라서.
struct IPool {
public:
    virtual ~IPool() = default;
    virtual void RemoveEntity(int entityId) = 0;
};

template <typename T>
class Pool : public IPool {
private:
    std::vector<T> mData;
    int mSize;

    std::unordered_map<int, int> mEntityIdToIndexMap;
    std::unordered_map<int, int> mIndexToEntityIdMap;

public:
    Pool(int cap = 100) {
        mSize = 0;
        mData.resize(cap);
    };
    virtual ~Pool() = default;

    // T& operator[](unsigned int index) {
    //     return mData[index];
    // }

    bool IsEmpty() const {
        return mSize == 0;
    }
    int GetSize() const {
        return mSize;
    }
    void Resize(int n) {
        mData.resize(n);
    }
    void Clear() {
        mData.clear();
        mSize = 0;
    }
    // void Add(const T object) {
    //     mData.push_back(object);
    // }
    void Set(int entityId, const T object) {
        if (mEntityIdToIndexMap.find(entityId) != mEntityIdToIndexMap.end()) {
            // 이미 존재하는 entity라면, 해당 entity의 index를 찾아서 대체.
            int idx = mEntityIdToIndexMap[entityId];
            mData[idx] = object;
        } else {
            int idx = mSize;
            mEntityIdToIndexMap.emplace(entityId, idx);
            mIndexToEntityIdMap.emplace(idx, entityId);
            if (idx >= mData.capacity()) {
                mData.resize(mSize * 2);
            }
            mData[idx] = object;
            mSize++;
        }
    }
    void Remove(int entityId) {
        // 지워져야 하는 자리에 마지막 요소를 넣는 작업
        int idxOfRemoved = mEntityIdToIndexMap[entityId];
        int idxOfLast = mSize - 1;
        mData[idxOfRemoved] = mData[idxOfLast]; // 마지막 요소를 지워져야 하는 자리에 넣어
                                                // pool을 pack하게 유지

        // entity-idx 매핑 업데이트
        int mEntityOfLastElement = mIndexToEntityIdMap[idxOfLast];
        mEntityIdToIndexMap[mEntityOfLastElement] = idxOfRemoved;
        mIndexToEntityIdMap[idxOfRemoved] = mEntityOfLastElement;

        // 제거
        mEntityIdToIndexMap.erase(entityId);  // 지워져야 하는 entity 삭제
        mIndexToEntityIdMap.erase(idxOfLast); // 마지막 entity 삭제

        mSize--;
    }
    void RemoveEntity(int entityId) override {
        if (mEntityIdToIndexMap.find(entityId) != mEntityIdToIndexMap.end()) {
            Remove(entityId);
        }
    }
    T& Get(int entityId) {
        return static_cast<T&>(mData[mEntityIdToIndexMap[entityId]]);
    }
};

////////////////////////////////////////////////////////
// registry
// 흔히 singleton으로 구현되는 game manager.
// ECS 시스템 간의 동작을 정의함.
////////////////////////////////////////////////////////
class Registry {
private:
    int mNumEntities = 0;

    /////////////////////////
    // tag, group
    /////////////////////////
    /*
        1 entity has 1 tag.
        1 tag has 1 entity.
        1:1

        1 group has many entities.
        1 entity has 1 group.
        1:N

        알고리즘 풀 때도 많이 나온 방식인데, 양방향 접근이 가능하도록 자료구조를 2개
       만드는 방식. 특히 여기선 메모리 조금 더 쓰고 속도를 얻는게 게임 엔진에선 훨씬 좋은
       선택이다.
    */

    std::unordered_map<TagName, Entity> mEntityPerTagMap;
    std::unordered_map<EntityId, TagName> mTagPerEntityMap;
    std::unordered_map<GroupName, std::set<Entity>> mEntityPerGroupMap;
    std::unordered_map<EntityId, GroupName> mGroupPerEntityMap;

    /////////////////////////
    // temp buffer
    /////////////////////////
    // 다음 tick의 Registry.Update를 타고 가도록 설정해놓는 일종의 buffer 역할
    // Update 도중 이러한 작업들이 일어나면 로직이 망가질 것이다.
    std::set<Entity> mEntitiesToBeAdded;
    std::set<Entity> mEntitiesToBeKilled;

    /////////////////////////
    // components
    /////////////////////////
    /*
        THIS COMMENT IS DEPRECATED BUT STILL USEFUL FOR UNDERSTANDING.

        vector of component pool.

        vector<vector<Component>> 의 꼴.

        [ Pool<Component0>, Pool<Component1>, Pool<Component2>, Pool<Component3] ]

        Pool<Component0> (for Comp0):
        [ Component0, Component0, Component0, ... ]
        index는 entityId에 해당.

        따라서, mComponentPools[componentId] 는 componentId에 해당하는 컴포넌트의
       배열이며, mComponentPools[componentId]의 index는 entityId를 가리킴.

        결과적으로,
        (entity의 갯수, component의 갯수)의 이차원 배열이 형성된다.
    */
    std::vector<std::shared_ptr<IPool>> mComponentPools;

    // mEntityComponentSignatures[entityId] = componentSignature
    // 특정 entity가 포함된 component를 찾을 수 있다.
    std::vector<Signature> mEntityComponentSignatures; // vector of component signature per entity.

    // 각 시스템의 목록.
    std::unordered_map<std::type_index, std::shared_ptr<System>> mSystemsMap;

    // killing된 entity의 목록
    std::deque<int> mFreeIds;

public:
    Registry();
    virtual ~Registry();

    void Update();

    /////////////////////////
    // entity
    /////////////////////////
    Entity CreateEntity();
    void KillEntity(Entity entity);

    /////////////////////////
    // tag
    /////////////////////////
    void TagEntity(Entity entity, const TagName& tag);
    bool EntityHasTag(Entity entity, const TagName& tag) const;
    Entity GetEntityByTag(const TagName& tag) const;
    void RemoveEntityTag(Entity entity);

    void GroupEntity(Entity entity, const GroupName& group);
    bool EntityBelongsToGroup(Entity entity, const GroupName& group) const;
    std::vector<Entity> GetEntitiesByGroup(const GroupName& group) const;
    void RemoveEntityFromGroup(Entity entity);

    /////////////////////////
    // component
    /////////////////////////
    // entity에 특정 component를 추가합니다.
    template <typename TComponent, typename... TArgs>
    void AddComponent(Entity entity, TArgs&&... args);

    // entity에 특정 component를 삭제합니다
    template <typename TComponent>
    void RemoveComponent(Entity entity);

    // entity에 특정 component가 존재하는지 확인합니다.
    template <typename TComponent>
    bool HasComponent(Entity entity) const;

    // entity의 component를 확인합니다.
    template <typename TComponent>
    TComponent& GetComponent(Entity entity) const;

    /////////////////////////
    // system
    /////////////////////////

    // 시스템을 추가, 삭제, 조회 등.
    template <typename TSystem, typename... TArgs>
    void AddSystem(TArgs&&... args);

    template <typename TSystem>
    void RemoveSystem();

    template <typename TSystem>
    bool HasSystem() const;

    template <typename TSystem>
    TSystem& GetSystem();

    // check signature and add, remove entity to system
    void AddEntityToSystems(Entity entity);
    void RemoveEntityFromSystems(Entity entity);
};

/*
    caution
    템플릿은 제네릭과 다르게 컴파일 시간에 인스턴스화 됨.
    따라서 템플릿의 구현을 별도의 .cpp 파일에 넣으면,
    컴파일러가 다른 소스 파일에서 해당 템플릿을 인스턴스화할 때 필요한 정보를 찾을 수 없게
   됨. 따라서 링크 오류가 발생함.
*/

template <typename TComponent, typename... TArgs>
inline void Registry::AddComponent(Entity entity, TArgs&&... args) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= mComponentPools.size()) {
        // 포인터를 담고 있으므로 nullptr 초기화 필요
        mComponentPools.resize(componentId + 1, nullptr);
    }

    // 만약 새로 생성된 컴포넌트라면.
    if (!mComponentPools[componentId]) {

        // newComponentPool은 vector<Component> 꼴.
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();

        mComponentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool =
        std::static_pointer_cast<Pool<TComponent>>(mComponentPools[componentId]);

    // std::forward
    // https://en.cppreference.com/w/cpp/utility/forward
    // forward args to create new component
    TComponent newComponent(std::forward<TArgs>(args)...);
    componentPool->Set(entityId, newComponent);

    // entity의 signature에 특정 componentId에 해당하는 bit를 올려준다.
    mEntityComponentSignatures[entityId].set(componentId);

    Logger::Log("component id: " + std::to_string(componentId) +
                " was added to entity id : " + std::to_string(entityId));
}

// entity에 특정 component를 삭제합니다
template <typename TComponent>
inline void Registry::RemoveComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(mComponentPools[componentId]);
    componentPool->Remove(entityId);

    mEntityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("component id: " + std::to_string(componentId) +
                " was removed from entity id : " + std::to_string(entityId));
}

// entity에 특정 component가 있는지 확인한다.
template <typename TComponent>
inline bool Registry::HasComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return mEntityComponentSignatures[entityId].test(componentId);
}

// entity가 소유한 특정 component(TComponent) 찾기
template <typename TComponent>
inline TComponent& Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // vector<Component>, index는 entityId.
    // [Component, Component, ...]
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(mComponentPools[componentId]);

    return componentPool->Get(entityId);
}

template <typename TSystem, typename... TArgs>
inline void Registry::AddSystem(TArgs&&... args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);

    // typeid
    // https://en.cppreference.com/w/cpp/language/typeid
    // typeid는 런타임에 객체의 타입 정보를 얻어오는데 사용 됩니다.
    // 동적 바인딩처럼, 실제 타입에 대한 정보를 반환합니다.
    mSystemsMap.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
inline void Registry::RemoveSystem() {
    auto system = mSystemsMap.find(std::type_index(typeid(TSystem)));
    mSystemsMap.erase(system);
}

template <typename TSystem>
inline bool Registry::HasSystem() const {
    return mSystemsMap.find(std::type_index(typeid(TSystem))) != mSystemsMap.end();
}

template <typename TSystem>
inline TSystem& Registry::GetSystem() {
    auto system = mSystemsMap.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename... TArgs>
inline void Entity::AddComponent(TArgs&&... args) {
    mRegistry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
inline void Entity::RemoveComponent() {
    mRegistry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
inline bool Entity::HasComponent() const {
    return mRegistry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
inline TComponent& Entity::GetComponent() const {
    return mRegistry->GetComponent<TComponent>(*this);
}
