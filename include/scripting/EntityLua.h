#pragma once
#include <entt/entity/entity.hpp>
#include <sol/sol.hpp>

namespace bee
{
class EntityLua
{
public:
    EntityLua(const entt::entity& entity);

    entt::entity m_entityID;

private:
};

namespace entity_lua
{
void bind_entity_class(sol::state& lua);

void inline bind(sol::state& lua) { bind_entity_class(lua); }
}
}
