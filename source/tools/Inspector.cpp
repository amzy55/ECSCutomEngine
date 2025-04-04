#include "tools/Inspector.h"

#include <glm/gtc/type_ptr.hpp>
#include <tools/tools.hpp>

#include "core/engine.hpp"
#include "ecs/ECS.h"
#include "ecs/components/compact_includes/AllComponents.h"

template <>
void MM::ComponentEditorWidget<bee::EmitterComponent>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& component = reg.get<bee::EmitterComponent>(e);
    ImGui::SliderFloat("Emit rate", &component.particlesPerSecond, 0.f, 5000.f);
}

template <>
void MM::ComponentEditorWidget<bee::TransformComponent>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& component = reg.get<bee::TransformComponent>(e);
    ImGui::SliderFloat3("Translation", glm::value_ptr(component.pos), -5.f, 5.f);
    glm::vec3 rotation = component.GetRotationInDegrees();
    ImGui::SliderFloat3("Rotation", value_ptr(rotation), -90.f, 90.f);
    component.rotation = bee::GetQuatFromDegrees(rotation);
    ImGui::SliderFloat3("Scale", value_ptr(component.scale), 0.f, 5.f);
}

template <>
void MM::ComponentEditorWidget<bee::NameComponent>(entt::registry& reg, entt::registry::entity_type e)
{
    const auto& component = reg.get<bee::NameComponent>(e);
    ImGui::Text("%s", component.name.c_str());
}

template <>
void MM::ComponentEditorWidget<bee::MeshComponent>(entt::registry& reg, entt::registry::entity_type e)
{
    auto& component = reg.get<bee::MeshComponent>(e);
    //ImGui::Text("Pointer: %d", component.mesh);
    ImGui::ColorEdit4("Add color", glm::value_ptr(component.addColor));
    ImGui::ColorEdit4("Mul color", glm::value_ptr(component.mulColor));
}

template <>
void MM::ComponentEditorWidget<bee::HierarchyComponent>(entt::registry& reg, entt::registry::entity_type e)
{
    const auto& component = reg.get<bee::HierarchyComponent>(e);

    if (component.parent != entt::null)
        ImGui::Text("Parent: %d", entt::to_integral(component.parent));
    else
        ImGui::Text("No parent");

    if (!component.children.empty())
        ImGui::Text("Children:");
    else
        ImGui::Text("No children");

    int childIndex = 0;
    for (const auto& child : component.children)
    {
        ImGui::Text("%d ", entt::to_integral(child));
        if (++childIndex % 5 != 0)
            ImGui::SameLine();
    }
}

bee::Inspector::Inspector()
{
    m_editor.registerComponent<bee::EmitterComponent>("Emitter");
    m_editor.registerComponent<bee::ParticleComponent>("Particle");
    m_editor.registerComponent<bee::TransformComponent>("Transform");
    m_editor.registerComponent<bee::HierarchyComponent>("Hierarchy");
    m_editor.registerComponent<bee::RootComponent>("Root");
    m_editor.registerComponent<bee::NameComponent>("Name");
    m_editor.registerComponent<bee::MeshComponent>("Mesh");
}

void bee::Inspector::Render() { m_editor.renderSimpleCombo(bee::Engine.ECS().Registry(), m_entityToEdit); }
