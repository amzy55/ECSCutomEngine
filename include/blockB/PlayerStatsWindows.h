#pragma once
#include <string>

#include "core/input.hpp"
#include "ecs/components/PlayerStatsComponents.h"

namespace bee
{
struct AbilitiesOnPlayerComponent;
struct PlayerStatsComponent;

class PlayerStatsWindows
{
public:
    /**
     * \brief Initializes the "selected items" members based on how many players there are.
     */
    void Init();
    void Display();

private:
    bool m_abilitiesTab = false;

    void GlobalPlayerSettingsWindow();

    void PlayerStatsRow(std::string header, float current, float base);
    void StatEffectsRow(
        const PlayerStatsComponent& playerComponent, PlayerStatsComponent::State statusEffect, int& columnIndex);
    void CenteredTitle(const char* title);
    void Info(const PlayerStatsComponent& playerComponent, AbilitiesOnPlayerComponent& abilities);
    void Edit(PlayerStatsComponent& playerComponent, AbilitiesOnPlayerComponent& abilities);
    void Abilities(const PlayerStatsComponent& playerComponent, AbilitiesOnPlayerComponent& abilities);
    void AbilitiesContent(const PlayerStatsComponent& playerComponent, AbilitiesOnPlayerComponent& abilities);

    std::vector<std::string> m_editAbilitySelectedItem;
    std::vector<std::pair<Input::KeyboardKey, std::string>> m_keyboardSelectedItem;
    std::vector<std::pair<Input::GamepadButton, std::string>> m_controllerSelectedItem;
};
}
