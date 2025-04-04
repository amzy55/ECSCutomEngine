#pragma once
#include "core/engine.hpp"
#include "ecs/ECS.h"

namespace bee {
class EntityLua;
}

namespace sol
{
class state;
}

namespace bee::components_lua
{
void bind_transform(sol::state& lua);
void bind_camera(sol::state& lua);

template<typename T>
T* get_component(const entt::entity& entity)
{
    return Engine.ECS().Registry().get<T>(entity);
}

template<typename T>
void bind_get_component(sol::state& lua, const std::string& userTypeName)
{
    //lua.sol::usertype<EntityLua>.set_function("Get" + userTypeName, get_component<T>());
}


void inline bind(sol::state& lua)
{
    bind_transform(lua);
    bind_camera(lua);
}
} //namespace bee::components_lua