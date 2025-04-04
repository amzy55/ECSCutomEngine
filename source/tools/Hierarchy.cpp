#include "tools/Hierarchy.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "ecs/components/compact_includes/HierarchyIncludeComponents.h"
#include "tools/tools.hpp"
#include "core/engine.hpp"
#include "ecs/ECS.h"
#include "ecs/components/PhysicsBody2DComponent.h"
#include "tools/log.hpp"
#include "resource_managers/ResourceManager.h"
#include "tools/GLTFLoader.h"
#include "tools/MainMenuBar.h"

bee::Hierarchy::Hierarchy(entt::registry& registry)
    : m_registry(registry)
{}

void bee::Hierarchy::DisplayHierarchyAndCalculateTransforms()
{
    auto& rootTransformComponent = m_registry.get<TransformComponent>(m_theoreticalRoot);
    // update world transform based on local transform
    rootTransformComponent.worldTransform = rootTransformComponent.GetTransformMatrix();
    CalculateTransforms(m_theoreticalRoot);

    if (!Engine.MainMenuBar().IsFlagOn(WindowsToDisplay::Hierarchy))
        return;

    ImGui::SetNextWindowSize({350.f, 600.f}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({0.f, 18.f}, ImGuiCond_Once);
    ImGui::Begin("Hierarchy");
    DisplayHierarchy(m_theoreticalRoot);
    ImGui::End();

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f); // default text color

    DisplayEditor();
}

void bee::Hierarchy::DisplayHierarchy(const entt::entity& parent, const int level)
{
    auto& hierarchyComponent = m_registry.get<HierarchyComponent>(parent);
    const auto& nameComponent = m_registry.try_get<NameComponent>(parent);
    std::string name;
    if (nameComponent)
    {
        name = nameComponent->name;
    }
    const std::string id = name + " " + std::to_string(entt::to_integral(parent));
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = m_textColors[level % m_textColors.size()];

    ImGui::PushID(id.c_str());
    ImGuiTreeNodeFlags treeNodeFlags =
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
    if (hierarchyComponent.children.empty())
    {
        treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }
    const bool open = ImGui::TreeNodeEx(id.c_str(), treeNodeFlags);
    if (ImGui::IsItemClicked())
    {
        hierarchyComponent.isSelected = true;
        m_registry.get<HierarchyComponent>(m_selectedNode).isSelected = false;
        m_selectedNode = parent;
    }
    if (open)
    {
        for (auto& child : hierarchyComponent.children)
        {
            DisplayHierarchy(child, level + 1);
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void bee::Hierarchy::CalculateTransforms(const entt::entity& parent)
{
    const auto& hierarchyComponent = m_registry.get<HierarchyComponent>(parent);
    if (hierarchyComponent.children.empty() == false)
    {
        for (auto& child : hierarchyComponent.children)
        {
            const auto& parentTransformComponent = m_registry.get<TransformComponent>(parent);
            auto& childTransformComponent = m_registry.get<TransformComponent>(child);
            childTransformComponent.worldTransform =
                parentTransformComponent.worldTransform * childTransformComponent.GetTransformMatrix();

            CalculateTransforms(child);
        }
    }
}

void bee::Hierarchy::DisplayEditor()
{
    ImGui::SetNextWindowSize({350.f, 364.f}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({0.f, 618.f}, ImGuiCond_Once);
    ImGui::Begin("Editor");
    if (const auto& nameComponent = m_registry.try_get<NameComponent>(m_selectedNode))
    {
        if (ImGui::CollapsingHeader("Name", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("%s", nameComponent->name.c_str());
        }
    }
    if (ImGui::CollapsingHeader("Entity ID", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("%s", std::to_string(entt::to_integral(m_selectedNode)).c_str());
    }
    if (const auto& physicsBodyComponent = m_registry.try_get<PhysicsBody2DComponent>(m_selectedNode))
    {
        if (ImGui::CollapsingHeader("Physics Body", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat2("Position", glm::value_ptr(physicsBodyComponent->position));
        }
    }
    else if (const auto& transformComponent = m_registry.try_get<TransformComponent>(m_selectedNode))
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Translation", glm::value_ptr(transformComponent->pos));
            glm::vec3 rotation = transformComponent->GetRotationInDegrees();
            ImGui::DragFloat3("Rotation", value_ptr(rotation));
            transformComponent->rotation = bee::GetQuatFromDegrees(rotation);
            ImGui::DragFloat3("Scale", value_ptr(transformComponent->scale));
        }
    }
    if (const auto& meshComponent = m_registry.try_get<MeshComponent>(m_selectedNode))
    {
        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit4("Add color", glm::value_ptr(meshComponent->addColor));
            ImGui::ColorEdit4("Mul color", glm::value_ptr(meshComponent->mulColor));
        }
    }
    if (const auto& hierarchyComponent = m_registry.try_get<HierarchyComponent>(m_selectedNode))
    {
        if (ImGui::CollapsingHeader("Hierarchy", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (hierarchyComponent->parent != entt::null)
                ImGui::Text("Parent: %d", entt::to_integral(hierarchyComponent->parent));
            else
                ImGui::Text("No parent");

            if (hierarchyComponent->children.empty())
                ImGui::Text("No children");
            else
            {
                ImGui::Text("Children:");
                std::string children;
                for (const auto& child : hierarchyComponent->children)
                {
                    children += std::to_string(entt::to_integral(child)) + " ";
                }
                ImGui::TextWrapped("%s", children.c_str());
            }
        }
    }
    ImGui::NewLine();
    if (const auto model = Engine.ResourceManager().GetModelManager().GetFilePathFromEntity(m_selectedNode))
    {
        // Set button colors
        ImGui::PushStyleColor(ImGuiCol_Button, {0.65f, 0.15f, 0.15f, 1.f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.8f, 0.3f, 0.3f, 1.f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1.f, 0.2f, 0.2f, 1.f});
        if (ImGui::Button("Destroy", {60.f, 20.f}))
        {
            DestroyEntityAndChildren(m_selectedNode);
            Engine.ResourceManager().GetModelManager().RemoveEntity(m_selectedNode);
            auto& rootHierarchyComponent = m_registry.get<HierarchyComponent>(m_theoreticalRoot);
            for (auto it = rootHierarchyComponent.children.begin(); it != rootHierarchyComponent.children.end(); ++it)
            {
                if (*it == m_selectedNode)
                {
                    rootHierarchyComponent.children.erase(it);
                    break;
                }
            }
            m_registry.destroy(m_selectedNode);
            m_selectedNode = m_theoreticalRoot;
            rootHierarchyComponent.isSelected = true;
        }
        ImGui::PopStyleColor(3);
        if (ImGui::Button("Duplicate", {70.f, 20.f}))
        {
            Engine.GLTFLoader().ConstructOrDuplicate(model.value());
        }
    }
    ImGui::End();
}

void bee::Hierarchy::DestroyEntityAndChildren(entt::entity& parent)
{
    auto& hierarchyComponent = m_registry.get<HierarchyComponent>(parent);
    for (auto& entity : hierarchyComponent.children)
    {
        DestroyEntityAndChildren(entity);
        m_registry.destroy(entity);
    }
}

void bee::Hierarchy::Init()
{
    const auto rootView = m_registry.view<TheoreticalRootComponent>();
    if (rootView.front() != entt::null) // theoretical root was serialized
    {
        m_theoreticalRoot = rootView.front();
        auto& rootHierarchyComponent = m_registry.get<HierarchyComponent>(m_theoreticalRoot);
        rootHierarchyComponent.children.clear();
        const auto view = m_registry.view<HierarchyComponent, RootComponent>();
        for (auto [entity, hierarchyComponent] : view.each())
        {
            rootHierarchyComponent.children.push_back(entity);
        }
    }
    else
    {
        m_theoreticalRoot = m_registry.create();
        auto& rootHierarchyComponent = m_registry.emplace<HierarchyComponent>(m_theoreticalRoot);
        m_registry.emplace<TransformComponent>(m_theoreticalRoot);
        auto& [rootName] = m_registry.emplace<NameComponent>(m_theoreticalRoot);
        rootName = "Entire scene";
        const auto view = m_registry.view<HierarchyComponent, RootComponent>();
        for (auto [entity, hierarchyComponent] : view.each())
        {
            hierarchyComponent.parent = m_theoreticalRoot;
            rootHierarchyComponent.children.push_back(entity);
        }
        m_registry.emplace<TheoreticalRootComponent>(m_theoreticalRoot);
        rootHierarchyComponent.isSelected = true;
    }
    const auto isSelectedView = m_registry.view<HierarchyComponent>();
    for (auto [entity, hierarchyComponent] : isSelectedView.each())
    {
        if (hierarchyComponent.isSelected == true)
        {
            m_selectedNode = entity;
            break;
        }
    } 
}

void bee::Hierarchy::SetSelected(entt::entity entity) { m_selectedNode = entity; }
