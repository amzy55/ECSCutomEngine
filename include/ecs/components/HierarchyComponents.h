#pragma once
#include <vector>
#include <entt/entity/entity.hpp>

namespace bee
{

struct HierarchyComponent
{
    entt::entity parent{entt::null};
    std::vector<entt::entity> children;
    bool isSelected = false;

    // if you want an entity to appear in the hierarchy
    // as a child of the theoretical root, the entity must have the RootComponent
    // and the parent of the new entity and the children of the theoretical root must be updated

    //auto theoreticalRootView = registry.view<TheoreticalRootComponent>();
    //auto& rootHierarchyComponent = registry.get<HierarchyComponent>(theoreticalRootView.front());
    //auto& newEntityHierarchyComponent = registry.get<HierarchyComponent>(newEntity);
    //newEntityHierarchyComponent.parent = theoreticalRootView.front();
    //rootHierarchyComponent.children.push_back(newEntity);

    // if you want it to have a mesh, besides the MeshComponent it must also have
    // TransformComponent and MeshRenderComponent
};

struct RootComponent // direct children of the theoretical root
{
};

struct TheoreticalRootComponent // the parent of all the nodes
{
};

}
