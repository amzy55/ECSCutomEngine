#include "blockB/Scene.h"

#include <imgui.h>

#include "core/engine.hpp"
#include "core/input.hpp"
#include "resource_managers/ResourceManager.h"
#include "ecs/ECS.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/PhysicsBody2DComponent.h"
#include "ecs/components/PlayerStatsComponents.h"
#include "ecs/components/compact_includes/HierarchyIncludeComponents.h"
#include "platform/pc/core/device_pc.hpp"
#include "tools/GLTFLoader.h"
#include "tools/tools.hpp"

using namespace bee;

// if the integer of the pairs is -1 then it will pass to the getter functions only the string
entt::entity create_entity_for_my_scene(
    const std::string name, const std::pair<std::string, int> meshInfo, const std::pair<std::string, int> textureInfo,
    const glm::vec4 color)
{
    auto& registry = Engine.ECS().Registry();

    auto theoreticalRootView = registry.view<TheoreticalRootComponent>();
    auto& rootHierarchyComponent = registry.get<HierarchyComponent>(theoreticalRootView.front());

    auto entity = registry.create();
    auto& meshComponent = registry.emplace<MeshComponent>(entity);

    if (meshInfo.second == -1)
        meshComponent.mesh = Engine.ResourceManager().GetMeshManager().Get(meshInfo.first).value();
    else
        meshComponent.mesh = Engine.ResourceManager().GetMeshManager().Get(meshInfo).value();

    if (textureInfo.second == -1)
        meshComponent.texture = Engine.ResourceManager().GetTextureManager().Get(textureInfo.first).value();
    else
        meshComponent.texture = Engine.ResourceManager().GetTextureManager().Get(textureInfo).value();

    meshComponent.mulColor = color;
    registry.emplace<RootComponent>(entity);
    registry.emplace<TransformComponent>(entity);
    auto& hierarchyComponent = registry.emplace<HierarchyComponent>(entity);
    hierarchyComponent.parent = theoreticalRootView.front();
    rootHierarchyComponent.children.push_back(entity);
    registry.emplace<NameComponent>(entity, name);

    return entity;
}

bee::Scene::Scene(entt::registry& registry)
    : m_registry(registry)
{
    Engine.ResourceManager().GetTextureManager().Add("textures/checkered.png");
}

void bee::Scene::Init()
{
    Engine.GLTFLoader().Load("assets/models/mycube.gltf");
    Engine.GLTFLoader().Load("assets/models/cylinder.gltf");

    const auto playerView = Engine.ECS().Registry().view<PlayerStatsComponent>();
    if (playerView.empty()) // no serialized players
    {
        std::string mycubePath = "assets/models/mycube.gltf";
        auto floorEnt =
            create_entity_for_my_scene("floor", {mycubePath, 0}, {"textures/checkered.png", -1}, {0.75f, 1.f, 0.75f, 1.0f});
        auto& floorTransform = m_registry.get<TransformComponent>(floorEnt);
        floorTransform.scale = glm::vec3(100.f, 0.1f, 100.f);
        floorTransform.pos = glm::vec3(0.f, -0.1f, 0.f);

        for (auto i = 0; i < 4; i++)
        {
            glm::vec4 color;
            if (i % 2 == 0)
            {
                color = glm::vec4{0.5f, 0.75f, 1.f, 1.0f};
            }
            else
            {
                color = glm::vec4{1.f, 0.5f, 0.75f, 1.f};
            }
            entt::entity player =
                create_entity_for_my_scene("player" + std::to_string(i + 1), {mycubePath, 0}, {mycubePath, 0}, color);

            m_registry.emplace<PlayerStatsComponent>(player, i, i % 2);
            m_registry.emplace<AbilitiesOnPlayerComponent>(player);

            auto& body = m_registry.emplace<PhysicsBody2DComponent>(player);
            body.position = {static_cast<float>(i) * 10.f - 15.f, 0.f};
            body.scale = 2.f;

            auto& transformPos = m_registry.get<TransformComponent>(player);
            transformPos.pos.y = body.scale;
            transformPos.scale.y = body.scale;
        }
    }
    m_playerStatsWindows.Init();
}

void bee::Scene::InputHandling()
{
    const auto& input = Engine.Input();

    if (input.GetMouseButton(Input::MouseButton::Right))
        return;

    if (input.GetKeyboardKeyOnce(Input::KeyboardKey::LeftAlt))
    {
        m_player1and2Input = !m_player1and2Input;
    }

    auto playerView = m_registry.view<PlayerStatsComponent, PhysicsBody2DComponent>();
    for (auto [entity, playerComponent, physicsBodyComponent] : playerView.each())
    {
        if (playerComponent.state & PlayerStatsComponent::State::Dashing)
        {
            continue;
        }
        if (playerComponent.state & PlayerStatsComponent::State::Rooted ||
            playerComponent.state & PlayerStatsComponent::State::Stunned)
        {
            physicsBodyComponent.velocity = glm::vec2(0.f);
            continue;
        }

        const float moveBy = playerComponent.currentMovementSpeed;
        auto velocity = glm::vec2(0.0f);

        // keyboard
        if (playerComponent.id == 0 && m_player1and2Input || playerComponent.id == 2 && !m_player1and2Input)
        {
            if (input.GetKeyboardKey(Input::KeyboardKey::A))
            {
                velocity.x -= moveBy;
            }
            if (input.GetKeyboardKey(Input::KeyboardKey::D))
            {
                velocity.x += moveBy;
            }
            if (input.GetKeyboardKey(Input::KeyboardKey::W))
            {
                velocity.y -= moveBy;
            }
            if (input.GetKeyboardKey(Input::KeyboardKey::S))
            {
                velocity.y += moveBy;
            }
        }
        else if (playerComponent.id == 1 && m_player1and2Input || playerComponent.id == 3 && !m_player1and2Input)
        {
            if (input.GetKeyboardKey(Input::KeyboardKey::ArrowLeft))
            {
                velocity.x -= moveBy;
            }
            if (input.GetKeyboardKey(Input::KeyboardKey::ArrowRight))
            {
                velocity.x += moveBy;
            }
            if (input.GetKeyboardKey(Input::KeyboardKey::ArrowUp))
            {
                velocity.y -= moveBy;
            }
            if (input.GetKeyboardKey(Input::KeyboardKey::ArrowDown))
            {
                velocity.y += moveBy;
            }
        }

        // controller stick
        const float axisX = input.GetGamepadAxis(playerComponent.id, Input::GamepadAxis::StickLeftX);
        const float axisY = input.GetGamepadAxis(playerComponent.id, Input::GamepadAxis::StickLeftY);

        if (axisX < -0.1f || axisX > 0.2f)
        {
            velocity.x += playerComponent.currentMovementSpeed * axisX;
        }
        if (axisY < -0.1f || axisY > 0.2f)
        {
            velocity.y += playerComponent.currentMovementSpeed * axisY;
        }

        // controller d-pad
        if (input.GetGamepadButton(playerComponent.id, Input::GamepadButton::DPadLeft))
        {
            velocity.x -= moveBy;
        }
        if (input.GetGamepadButton(playerComponent.id, Input::GamepadButton::DPadRight))
        {
            velocity.x += moveBy;
        }
        if (input.GetGamepadButton(playerComponent.id, Input::GamepadButton::DPadUp))
        {
            velocity.y -= moveBy;
        }
        if (input.GetGamepadButton(playerComponent.id, Input::GamepadButton::DPadDown))
        {
            velocity.y += moveBy;
        }

        physicsBodyComponent.velocity = velocity;
    }
}

void bee::Scene::Update()
{
    InputHandling();
    m_playerStatsWindows.Display();
    m_abilityCreationMenu.Display();
}

void Scene::DisplayUIOnPlayers()
{
    const auto& device = Engine.Device();
    const auto playerView = m_registry.view<PlayerStatsComponent, TransformComponent>();
    for (auto [entity, player, transform] : playerView.each())
    {
        const auto cameraView = m_registry.view<CameraComponent>();
        auto& cameraComponent = m_registry.get<CameraComponent>(cameraView.front());
        const glm::vec2 windowSize = {device.GetWidth(), device.GetHeight()};
        glm::vec2 screenPos =
            WorldToScreenCoordinates(transform.worldTransform, cameraComponent.view, cameraComponent.projection, windowSize);
        ImGui::SetNextWindowPos({screenPos.x, screenPos.y});
        ImGui::Begin(std::to_string(player.id).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Test");
        ImGui::End();
    }
}
