#pragma once

#include "Components/BoxColliderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"
#include "glm/glm.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <memory>

class RenderGUISystem : public System {
public:
    RenderGUISystem() = default;
    virtual ~RenderGUISystem() = default;

    void Update(const std::unique_ptr<Registry>& registry) {
        ImGui::NewFrame();

        if (ImGui::Begin("Test Window")) {

            // 매번 새롭게 render되기 때문에 메모리에서 해제되지 않게 static으로 해야 함.
            static int enemyPosX = 0;
            static int enemyPosY = 0;

            ImGui::InputInt("enemy pos x", &enemyPosX);
            ImGui::InputInt("enemy pos y", &enemyPosY);

            if (ImGui::Button("create new enemy")) {
                Entity enemy = registry->CreateEntity();
                enemy.Group("enemies");
                enemy.AddComponent<TransformComponent>(glm::vec2(enemyPosX, enemyPosY), glm::vec2(1.0, 1.0), 0.0);
                enemy.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 00.0));
                enemy.AddComponent<SpriteComponent>("tank-image", 32, 32, ZIndex::Enemy);
                enemy.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 1000, 10000, 10, false);
                enemy.AddComponent<HealthComponent>(100);
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }
};