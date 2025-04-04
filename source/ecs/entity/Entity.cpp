#include "ecs/entity/Entity.h"

#include "core/engine.hpp"
#include "ecs/ECS.h"

bee::Entity::Entity(const entt::entity entity) : m_entity(entity) {}

template <typename T>
T& bee::Entity::GetComponent()
{
    return Engine.ECS().Registry().get<T>(m_entity);
}

template <typename T, typename... Args>
T& bee::Entity::AddComponent(Args&&... args)
{
    return Engine.ECS().Registry().emplace<T>(m_entity, std::forward<Args>(args)...);
}

template <typename T>
void bee::Entity::RemoveComponent()
{
    bee::Engine.ECS().Registry().remove<T>(m_entity);
}