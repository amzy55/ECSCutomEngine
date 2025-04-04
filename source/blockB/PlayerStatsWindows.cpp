#include "blockB/PlayerStatsWindows.h"

#include <imgui.h>

#include "core/engine.hpp"
#include "core/input.hpp"
#include "ecs/ECS.h"
#include "ecs/components/PlayerStatsComponents.h"
#include "resource_managers/ResourceManager.h"
#include "tools/ImGuiHelpers.h"
#include "tools/MainMenuBar.h"

void bee::PlayerStatsWindows::Init()
{
    const auto playerView = Engine.ECS().Registry().view<PlayerStatsComponent>();
    for (auto entity : playerView)
    {
        m_editAbilitySelectedItem.emplace_back("");
        m_keyboardSelectedItem.emplace_back(Input::KeyboardKey::Space, "");
        m_controllerSelectedItem.emplace_back(Input::GamepadButton::South, "");
    }
}

void bee::PlayerStatsWindows::Display()
{
    if (!Engine.MainMenuBar().IsFlagOn(WindowsToDisplay::PlayerStats))
        return;

    const auto playerView = Engine.ECS().Registry().view<PlayerStatsComponent, AbilitiesOnPlayerComponent>();
    for (auto [entity, playerComponent, abilities] : playerView.each())
    {
        std::string windowName = "Player " + std::to_string(playerComponent.id + 1);
        ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginTabBar((windowName + "Tab Bar").c_str()))
        {
            Info(playerComponent, abilities);
            Edit(playerComponent, abilities);
            if (m_abilitiesTab)
                Abilities(playerComponent, abilities);
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
    if (const auto& input = Engine.Input(); 
        input.GetKeyboardKey(Input::KeyboardKey::LeftShift) &&
        input.GetKeyboardKeyOnce(Input::KeyboardKey::Tab))
    {
        m_abilitiesTab = !m_abilitiesTab;
    }
}

void bee::PlayerStatsWindows::GlobalPlayerSettingsWindow()
{
    //ImGui::Begin("Global Player Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

    //if (ImGui::BeginTabBar("Tab Bar"))
    //{
    //    Info(playerComponent, abilities);
    //    Edit(playerComponent, abilities);
    //    if (m_abilitiesTab)
    //        Abilities(playerComponent, abilities);
    //    ImGui::EndTabBar();
    //}
    //ImGui::End();
}

void bee::PlayerStatsWindows::PlayerStatsRow(std::string header, float current, float base)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", header.c_str());
    ImGui::TableNextColumn();
    ImGui::Text("%.1f", current);
    ImGui::TableNextColumn();
    ImGui::Text("%.1f", base);
}

void bee::PlayerStatsWindows::StatEffectsRow(
    const PlayerStatsComponent& playerComponent, PlayerStatsComponent::State statusEffect, int& columnIndex)
{
    ImGui::TableNextColumn();
    ImGui::NewLine();
    ImGui::SameLine(ImGui::GetColumnWidth(columnIndex) * 0.5f);
    const char* checkMark = playerComponent.IsStatusEffectActive(statusEffect) ? "X" : "";
    ImGui::Text("%s", checkMark);
    columnIndex++;
}

void bee::PlayerStatsWindows::CenteredTitle(const char* title)
{
    ImVec2 statsTitlesSize = ImGui::CalcTextSize(title);
    ImGui::NewLine();
    ImGui::SameLine((ImGui::GetWindowWidth() - statsTitlesSize.x) * 0.5f);
    ImGui::Text("%s", title);
}

void bee::PlayerStatsWindows::Info(const PlayerStatsComponent& playerComponent, AbilitiesOnPlayerComponent& abilities)
{
    if (ImGui::BeginTabItem("Info"))
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.f, 0.2f, 0.1f, 1.0f);
        ImGui::Text("Health");
        ImGui::SameLine();
        const auto healthUpperLimit = std::max(playerComponent.currentHealth, playerComponent.baseHealth);
        std::string healthProgressBarText = std::to_string(static_cast<int>(playerComponent.currentHealth)) + "/" +
                                            std::to_string(static_cast<int>(healthUpperLimit));
        ImGui::ProgressBar(playerComponent.currentHealth / healthUpperLimit, ImVec2(0.0f, 0.0f), healthProgressBarText.c_str());

        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.f, 0.5f, 0.0f, 1.0f);
        ImGui::Text("Ammo  ");
        for (float i = 0.f; i < playerComponent.baseAmmo; i += 1.f)
        {
            ImGui::SameLine();
            ImGui::ProgressBar((playerComponent.currentAmmo - i) / 1.0f, ImVec2(30.0f, 0.0f), "");
        }

        ImGui::Separator();
        CenteredTitle("Stats");
        if (ImGui::BeginTable("Stats", 3))
        {
            // Setup headers for columns
            ImGui::TableSetupColumn("Stat"); // Pseudo-header column
            ImGui::TableSetupColumn("Current");
            ImGui::TableSetupColumn("Base");
            ImGui::TableHeadersRow();

            PlayerStatsRow("Health", playerComponent.currentHealth, playerComponent.baseHealth);
            PlayerStatsRow("Ammo", playerComponent.currentAmmo, playerComponent.baseAmmo);
            PlayerStatsRow("Reload Speed", playerComponent.currentReloadSpeed, playerComponent.baseReloadSpeed);
            PlayerStatsRow("Movement Speed", playerComponent.currentMovementSpeed, playerComponent.baseMovementSpeed);
            PlayerStatsRow(
                "Dealt Damage Modifier %", playerComponent.currentDealtDamageModifier * 100.f,
                playerComponent.baseDealtDamageModifier * 100.f);
            PlayerStatsRow(
                "Received Damage Reduction %", (playerComponent.currentReceivedDamageReduction - 1.f) * 100.f,
                (playerComponent.baseReceivedDamageReduction - 1.f) * 100.f);

            ImGui::EndTable();
        }

        ImGui::Separator();
        CenteredTitle("Status Effects");
        if (ImGui::BeginTable("Status Effects", 4))
        {
            // Setup headers for columns
            ImGui::TableSetupColumn("Stunned");
            ImGui::TableSetupColumn("Rooted");
            ImGui::TableSetupColumn("Silenced");
            ImGui::TableSetupColumn("Invulnerable");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();

            int columnIndex = 0;
            StatEffectsRow(playerComponent, PlayerStatsComponent::State::Stunned, columnIndex);
            StatEffectsRow(playerComponent, PlayerStatsComponent::State::Rooted, columnIndex);
            StatEffectsRow(playerComponent, PlayerStatsComponent::State::Silenced, columnIndex);
            StatEffectsRow(playerComponent, PlayerStatsComponent::State::Invincible, columnIndex);

            ImGui::EndTable();
        }
        if (!m_abilitiesTab)
            Abilities(playerComponent, abilities);
        ImGui::EndTabItem();
    }
}

void bee::PlayerStatsWindows::Edit(PlayerStatsComponent& playerComponent, AbilitiesOnPlayerComponent& abilities)
{
    if (ImGui::BeginTabItem("Edit Stats"))
    {
        if (ImGui::InputFloat("Health", &playerComponent.baseHealth))
        {
            playerComponent.baseHealth = std::max(0.f, playerComponent.baseHealth);
            playerComponent.currentHealth = playerComponent.baseHealth;
        }

        if (ImGui::InputFloat("Ammo", &playerComponent.baseAmmo))
        {
            playerComponent.baseAmmo = std::max(0.f, playerComponent.baseAmmo);
            playerComponent.currentAmmo = playerComponent.baseAmmo;
        }

        if (ImGui::InputFloat("Reload Speed", &playerComponent.baseReloadSpeed))
        {
            playerComponent.baseReloadSpeed = std::max(0.f, playerComponent.baseReloadSpeed);
            playerComponent.currentReloadSpeed = playerComponent.baseReloadSpeed;   
        }

        if (ImGui::InputFloat("Movement Speed", &playerComponent.baseMovementSpeed))
        {
            playerComponent.baseMovementSpeed = std::max(0.f, playerComponent.baseMovementSpeed);
            playerComponent.currentMovementSpeed = playerComponent.baseMovementSpeed;
        }

        float baseDealtDamageModifierPercentage = playerComponent.baseDealtDamageModifier * 100.f;
        if (ImGui::InputFloat("Dealt Damage Modifier %", &baseDealtDamageModifierPercentage))
        {
            baseDealtDamageModifierPercentage = std::max(0.f, baseDealtDamageModifierPercentage);
            playerComponent.baseDealtDamageModifier = baseDealtDamageModifierPercentage / 100.f;
            playerComponent.currentDealtDamageModifier = playerComponent.baseDealtDamageModifier;
        }

        float baseReceivedDamageReductionPercentage = (playerComponent.baseReceivedDamageReduction - 1.f) * 100.f;
        if (ImGui::InputFloat("Received Damage Modifier %", &baseReceivedDamageReductionPercentage))
        {
            baseReceivedDamageReductionPercentage = std::max(0.f, baseReceivedDamageReductionPercentage);
            playerComponent.baseReceivedDamageReduction = baseReceivedDamageReductionPercentage / 100.f + 1.f;
            playerComponent.currentReceivedDamageReduction = playerComponent.baseReceivedDamageReduction;
        }

        if (ImGui::InputInt("Max Number of Abilities", &abilities.maxNumberOfAbilities))
        {
            abilities.maxNumberOfAbilities = std::max(1, abilities.maxNumberOfAbilities);
            if (static_cast<int>(abilities.abilityNamesToInputsMap.size()) > abilities.maxNumberOfAbilities)
            {
                auto deleteStart = abilities.abilityNamesToInputsMap.begin();
                std::advance(deleteStart, abilities.maxNumberOfAbilities);
                abilities.abilityNamesToInputsMap.erase(deleteStart, abilities.abilityNamesToInputsMap.end());
            }
        }
        ImGui::EndTabItem();
    }
}

void bee::PlayerStatsWindows::Abilities(
    const PlayerStatsComponent& playerComponent, AbilitiesOnPlayerComponent& abilities)
{
    if (m_abilitiesTab)
    {
        if (ImGui::BeginTabItem("Abilities"))
        {
            AbilitiesContent(playerComponent, abilities);
            ImGui::EndTabItem();
        }
    }
    else
    {
        if (ImGui::CollapsingHeader("Abilities", ImGuiTreeNodeFlags_DefaultOpen))
        {
            AbilitiesContent(playerComponent, abilities);
        }
    }
}

void bee::PlayerStatsWindows::AbilitiesContent(
    const PlayerStatsComponent& playerComponent, AbilitiesOnPlayerComponent& abilities)
{
    const auto& abilitiesResourceManager = Engine.ResourceManager().GetAbilityManager().m_abilities;
    const auto count = static_cast<int>(abilities.abilityNamesToInputsMap.size());
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
    ImGui::Text("Count: %d/%d", count, abilities.maxNumberOfAbilities);
    ImGui::SameLine();
    if (ImGui::Button("+", {16.f, 16.f}))
    {
        if (count<abilities.maxNumberOfAbilities&& static_cast<int>(abilitiesResourceManager.size())> count)
        {
            bool foundNewAbility = false;
            for (const auto& [abilityNameResourceManager, abilitySettings] : abilitiesResourceManager)
            {
                for (auto& [abilityNamePlayer, input] : abilities.abilityNamesToInputsMap)
                {
                    if (abilityNamePlayer == abilityNameResourceManager)
                    {
                        foundNewAbility = false;
                        break;
                    }
                    else
                    {
                        foundNewAbility = true;
                    }
                }
                if (foundNewAbility || abilities.abilityNamesToInputsMap.empty())
                {
                    abilities.abilityNamesToInputsMap[abilityNameResourceManager] = {}; // nullopt
                    break;
                }
            }
        }
    }
    ImGui::SameLine();
    help_marker("A new ability can be added only if there are any new abilities in the resource manager.");
    if (!abilities.abilityNamesToInputsMap.empty())
    {
        int i = 0;
        std::string newAbility, abilityToDelete;
        for (auto& [abilityName, inputBindings] : abilities.abilityNamesToInputsMap)
        {
            ImGui::PushID(i);
            i++;
            const int playerID = playerComponent.id;
            ImGui::Separator();
            ImGui::Text("Ability %d", i);
            if (ImGui::BeginCombo("Ability", abilityName.c_str()))
            {
                for (const auto& [abilityNameResourceManager, abilitySettings] : abilitiesResourceManager)
                {
                    const bool isSelected = (m_editAbilitySelectedItem[playerID] == abilityNameResourceManager);

                    if (ImGui::Selectable(abilityNameResourceManager.c_str(), isSelected) &&
                        abilityName != abilityNameResourceManager)
                    {
                        m_editAbilitySelectedItem[playerID] = abilityNameResourceManager;
                        newAbility = abilityNameResourceManager;
                        abilityToDelete = abilityName;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Input");
            const auto& [keyboardInputToStrings, controllerInputToStrings] = Engine.Input().inputToStrings;
            std::string previewValue =
                inputBindings.first.has_value() ? keyboardInputToStrings.at(inputBindings.first.value()) : "";
            if (ImGui::BeginCombo("Keyboard", previewValue.c_str()))
            {
                for (const auto& [keyboardKey, string] : keyboardInputToStrings)
                {
                    const bool isSelected = (m_keyboardSelectedItem[playerID].first == keyboardKey);

                    if (ImGui::Selectable(string.c_str(), isSelected))
                    {
                        m_keyboardSelectedItem[playerID] = std::make_pair(keyboardKey, string);
                        inputBindings.first = keyboardKey;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            previewValue = inputBindings.second.has_value() ? controllerInputToStrings.at(inputBindings.second.value()) : "";
            if (ImGui::BeginCombo("Controller", previewValue.c_str()))
            {
                for (const auto& [controllerButton, string] : controllerInputToStrings)
                {
                    const bool isSelected = (m_controllerSelectedItem[playerID].first == controllerButton);

                    if (ImGui::Selectable(string.c_str(), isSelected))
                    {
                        m_controllerSelectedItem[playerID] = std::make_pair(controllerButton, string);
                        inputBindings.second = controllerButton;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();
        }
        if (!newAbility.empty())
        {
            abilities.abilityNamesToInputsMap[newAbility] = abilities.abilityNamesToInputsMap[abilityToDelete];
            abilities.abilityNamesToInputsMap.erase(abilityToDelete);
        }
    }
}
