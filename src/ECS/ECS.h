#pragma once

#include <bitset>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;

// keep track of which components an entity has
// also, which entities a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

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
private:
    // return unique id of component
    // T가 다를 때 마다 새로운 Component class가 될 것이다.
    // Component<A>에 부여되는 static 변수와 Component<B> 클래스에 부여되는 static 변수는 다르다.
    static int GetId() {
        static auto mId = nextId++;
        return mId;
    }

public:
    Component();
    virtual ~Component();
};

////////////////////////////////////////////////////////
// Entity. just id.
////////////////////////////////////////////////////////
class Entity {
private:
    int mId;

public:
    Entity(int mId);
    virtual ~Entity();

    Entity(const Entity& rhs) = default;        // 복사 생성
    Entity& operator=(const Entity&) = default; // 복사 대입 연산

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

    int GetId() const;
};

////////////////////////////////////////////////////////
// system
// mutate state in component that entities has
////////////////////////////////////////////////////////
class System {
private:
    Signature mComponentSignature;
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

// 템플릿은 제네릭과 다르게 컴파일 시간에 인스턴스화 됨.
// 따라서 템플릿의 구현을 별도의 .cpp 파일에 넣으면,
// 컴파일러가 다른 소스 파일에서 해당 템플릿을 인스턴스화할 때 필요한 정보를 찾을 수 없게 됨.
// 따라서 링크 오류가 발생함.
template <typename TComponent>
inline void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    mComponentSignature.set(componentId);
};

////////////////////////////////////////////////////////
// registry
// 흔히 singleton으로 구현되는 game manager
////////////////////////////////////////////////////////
class Registry {
private:
    unsigned int mNumEntities = 0;

public:
    Registry();
    virtual ~Registry();

    Entity CreateEntity();
    void KillEntity();
    void AddComponent();
    void RemoveComponent();
    void AddSystem();
    void RemoveSystem();
};
