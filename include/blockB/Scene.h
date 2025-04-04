#pragma once
#include "entt/entity/registry.hpp"
#include "blockB/AbilityCreationMenu.h"
#include "blockB/PlayerStatsWindows.h"

namespace entt {
enum class entity : std::uint32_t;
}

namespace bee
{
class Scene
{
public:
    Scene(entt::registry& registry);

    void Init();
    void Update();
    void DisplayUIOnPlayers();

private:
    void InputHandling();

    entt::registry& m_registry;

    AbilityCreationMenu m_abilityCreationMenu;
    PlayerStatsWindows m_playerStatsWindows;

    bool m_player1and2Input = true;
};
}
