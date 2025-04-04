#include "ecs/systems/ScriptSystem.h"
#include "ecs/components/ScriptComponent.h"
#include "scripting/components_lua.h"
#include "scripting/EntityLua.h"
#include "tools/log.hpp"
#include "scripting/glm_lua.h"
#include "scripting/input_lua.h"

bee::ScriptSystem::ScriptSystem(entt::registry& registry) : BaseSystem(registry)
{
    m_lua.open_libraries(sol::lib::base);
    glm_lua::bind(m_lua);
    entity_lua::bind(m_lua);
    components_lua::bind(m_lua);
    input_lua::bind(m_lua);
}

void bee::ScriptSystem::Update(const float& dt)
{
    const auto view = m_registry.view<ScriptComponent>();
    for (auto [entity, scriptComponent] : view.each())
    {
        for (auto& script : scriptComponent.scripts)
        {
            auto result = m_lua.script_file(script, sol::script_pass_on_error);
            if (!result.valid())
            {
                const sol::error error = result;
                Log::Error(error.what());
            }
            else
            {
                sol::table type = result;
                sol::table object = type["new"]();
                object["update"](object, EntityLua(entity), dt);
            }
        }
    }
}
