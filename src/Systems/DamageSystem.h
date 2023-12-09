#pragma once

#include "Components/BoxColliderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ProjectileComponent.h"
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

    void OnProjectileHitsPlayer(Entity projectile, Entity player) {
        Logger::Log("damage system got projectile hits player event " + std::to_string(projectile.GetId()) + " and " +
                    std::to_string(player.GetId()));

        auto& projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (!projectileComponent.mIsFriendly) {
            auto& health = player.GetComponent<HealthComponent>();
            health.mHealthPercentage -= projectileComponent.mHitPercentDamage;

            projectile.Kill();

            if (health.mHealthPercentage <= 0) {
                player.Kill();
            }
        }
    }

    void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
        Logger::Log("damage system got projectile hits enemy event " + std::to_string(projectile.GetId()) + " and " +
                    std::to_string(enemy.GetId()));

        auto& projectileComponent = projectile.GetComponent<ProjectileComponent>();

        // player에게 friendly한 projectile이면 enemy에겐 데미지를 주어야 함.
        if (projectileComponent.mIsFriendly) {
            auto& health = enemy.GetComponent<HealthComponent>();
            health.mHealthPercentage -= projectileComponent.mHitPercentDamage;

            projectile.Kill();

            if (health.mHealthPercentage <= 0) {
                enemy.Kill();
            }
        }
    }

    void onCollision(CollisionEvent& event) {
        Entity a = event.a;
        Entity b = event.b;

        Logger::Log("damage system got collision event " + std::to_string(a.GetId()) + " and " +
                    std::to_string(b.GetId()));

        if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
            OnProjectileHitsPlayer(a, b);
        }
        if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
            OnProjectileHitsPlayer(b, a);
        }
        if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
            OnProjectileHitsEnemy(a, b);
        }
        if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
            OnProjectileHitsEnemy(b, a);
        }
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {

        // 함수 매개변수의 타입을 바탕으로 템플릿 매개변수의 타입을 추론가능.
        // 따라서 모든 typename을 명시할 필요는 없다.
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
    }

    void Update() {
    }
};