#pragma once
#include "BaseSystem.h"
#include "sol/sol.hpp"

// For explanations on how scripts should be written and used,
// please refer to the README file in the assets/scripts folder.

namespace bee
{
class ScriptSystem : public BaseSystem
{
public:
    explicit ScriptSystem(entt::registry& registry);

    /**
     * \brief Runs all scripts.
     * \param dt delta time
     */
    void Update(const float& dt) override;

private:
    sol::state m_lua;

};
}
