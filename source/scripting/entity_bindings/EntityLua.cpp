#include "scripting/EntityLua.h"

#include "core/engine.hpp"
#include "ecs/ECS.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/TransformComponent.h"

bee::EntityLua::EntityLua(const entt::entity& entity)
    :m_entityID(entity)
{}

void bee::entity_lua::bind_entity_class(sol::state& lua)
{
    sol::usertype<EntityLua> entity_table =
        lua.new_usertype<EntityLua>("EntityLua", sol::constructors<EntityLua(const entt::entity& entity)>());

    entity_table["GetTransform"] = [](const EntityLua& entity)
    { return Engine.ECS().Registry().try_get<TransformComponent>(entity.m_entityID); };
    entity_table["GetCamera"] = [](const EntityLua& entity)
    { return Engine.ECS().Registry().try_get<CameraComponent>(entity.m_entityID); };
}