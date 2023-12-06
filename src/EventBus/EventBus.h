#pragma once

#include "EventBus/Event.h"
#include "Logger/Logger.h"
#include <functional>
#include <list>
#include <map>
#include <typeindex>
#include <utility>

// ECS.h의 IPool과 마찬가지로, template class를 template으로 받아야 하는 상황에서 유용
class IEventCallback {
private:
    virtual void Call(Event& e) = 0; // this is pure virtual
public:
    virtual ~IEventCallback() = default;

    void Execute(Event& e) {
        Call(e);
    }
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {
private:
    // CallbackFunction is pointer to (TEvent&) -> void
    typedef void (TOwner::*CallbackFunction)(TEvent&);

    TOwner* mOwnerInstance;
    CallbackFunction mCallbackFunction;

    void Call(Event& e) override {
        // https://en.cppreference.com/w/cpp/utility/functional/invoke
        // std::invoke(callable, args...)
        std::invoke(mCallbackFunction, mOwnerInstance, static_cast<TEvent&>(e));
    }

public:
    EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction) {
        this->mOwnerInstance = ownerInstance;
        this->mCallbackFunction = callbackFunction;
    }

    virtual ~EventCallback() override = default;
};

// list of callback ptr.
typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {
private:
    /* 특정 이벤트에 물려 있는 callback의 map
        {
            "CollisionEvent": list[*cb1, *cb2],
            "WindowResizeEvent": list[*cb5, *cb9],
            ...
        }
    */
    std::map<std::type_index, std::unique_ptr<HandlerList>> mSubscribers;

public:
    EventBus() {
        Logger::Log("event bus constructor called");
    };
    virtual ~EventBus() {
        Logger::Log("event bus deconstructor called");
    };

    // map을 비움으로써 모든 이벤트 cb를 삭제합니다.
    void Reset() {
        mSubscribers.clear();
    }

    // 특정 이벤트를 구독합니다.
    // void (TOwner::*callbackFunction)(TEvent&)를 left-right rule에 의해 해석해보자면
    // TOwner::callbackFunction is a pointer to (TEvent&) -> void
    template <typename TEvent, typename TOwner>
    void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)) {
        if (!mSubscribers[typeid(TEvent)].get()) {
            mSubscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        auto handler = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        mSubscribers[typeid(TEvent)]->push_back(std::move(handler));
    }

    // 특정 이벤트를 발생시킵니다.
    template <typename TEvent, typename... TArgs>
    void EmitEvent(TArgs&&... args) {
        auto handlers = mSubscribers[typeid(TEvent)].get();
        if (handlers) {
            for (auto it = handlers->begin(); it != handlers->end(); it++) {
                auto handler = it->get();
                TEvent event(std::forward<TArgs>(args)...);
                handler->Execute(event);
            }
        }
    }
};
