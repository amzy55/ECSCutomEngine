#include "blockB/AbilityCreationMenu.h"

#include <imgui.h>

#include "blockB/AbilitySettings.h"
#include "core/engine.hpp"
#include "ecs/ECS.h"
#include "ecs/components/PlayerStatsComponents.h"
#include "resource_managers/ResourceManager.h"
#include "tools/MainMenuBar.h"
#include "tools/tools.hpp"
#include "tools/ImGuiHelpers.h"

void bee::AbilityCreationMenu::Display()
{
    if (!Engine.MainMenuBar().IsFlagOn(WindowsToDisplay::AbilityMenu))
        return;

    DisplayAbilityCreationSettingsWindow();
    DisplayWarningsWindow();
    DisplayConfirmOrOverrideWindow();
}

void bee::AbilityCreationMenu::DisplayAbilityCreationSettingsWindow()
{
    ImGui::SetNextWindowSize({500.f, 664.f}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({1420.f, 318.f}, ImGuiCond_Once);
    ImGui::Begin("Ability Creation Settings");

    NameSection();

    TargetingProfileSection();

    ApplyTypeSection();

    if (static_cast<InstantType>(m_instantTypesSelectedItem) == InstantType::Dash &&
        static_cast<Target>(m_targetsSelectedItem) == Target::Self)
    {
        DashSection();
    }
    else if (static_cast<ApplyType>(m_applyTypesSelectedItem) == ApplyType::StatusEffect)
    {
        // display nothing
    }
    else
    {
        EffectSection();
    }

    OnSaveClick();
    DisplayErrors();
    DisplayEditCombo();

    ImGui::End();
}

void bee::AbilityCreationMenu::DisplayWarningsWindow()
{
    if (m_displayWarningsWindow)
    {
        if (WarningsWindow())
        {
            DisplayConfirmOverrideWindowOrSaveOrOverrideAbility();
        }
    }
}
void bee::AbilityCreationMenu::DisplayConfirmOrOverrideWindow()
{
    if (m_displayConfirmOverrideWindow)
    {
        if (ConfirmOverrideWindow())
        {
            SaveOrOverrideAbility();
        }
    }
}

void bee::AbilityCreationMenu::NameSection()
{
    if (ImGui::CollapsingHeader("Name", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputText("Ability Name", m_name, IM_ARRAYSIZE(m_name));
    }
}

void bee::AbilityCreationMenu::TargetingProfileSection()
{
    if (ImGui::CollapsingHeader("Targeting Profile", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Combo("Target", &m_targetsSelectedItem, m_targets, IM_ARRAYSIZE(m_targets));
        tool_tip("Who does the ability affect?");

        if (static_cast<Target>(m_targetsSelectedItem) != Target::Self)
        {
            ImGui::Combo("Targeting Style", &m_castTypesSelectedItem, m_castTypes, IM_ARRAYSIZE(m_castTypes));
            tool_tip("How are the chosen targets targeted?");

            switch (static_cast<CastType>(m_castTypesSelectedItem))
            {
                case CastType::RadiusAroundPlayer:
                    ImGui::InputFloat("Radius", &m_aoeRadius);
                    tool_tip("Radius of the area of effect.");
                    break;
                case CastType::Projectile:
                    ImGui::InputInt("Number of Projectiles", &m_numberOfProjectiles);
                    m_numberOfProjectiles = std::max(m_numberOfProjectiles, 1);
                    if (m_numberOfProjectiles > 1)
                    {
                        ImGui::Combo(
                            "Shoot Style", &m_projectileShootTypesSelectedItem, m_projectileShootTypes,
                            IM_ARRAYSIZE(m_projectileShootTypes));
                        tool_tip("Are the projectiles shot in a line consecutively or in a cone simultaneously?");
                        switch (static_cast<ProjectileShootType>(m_projectileShootTypesSelectedItem))
                        {
                            case ProjectileShootType::Line:
                                ImGui::InputFloat("Space between Projectiles", &m_spaceBetweenProjectiles);
                                m_spaceBetweenProjectiles = std::max(m_spaceBetweenProjectiles, 0.f);
                                break;
                            case ProjectileShootType::Cone:
                                auto coneAngleInDegrees = glm::degrees(m_coneAngle);
                                ImGui::InputFloat("Cone Angle Degrees", &coneAngleInDegrees);
                                coneAngleInDegrees = std::clamp(coneAngleInDegrees, 0.f, 360.f);
                                m_coneAngle = glm::radians(coneAngleInDegrees);
                                break;
                        }
                    }
                    ImGui::InputFloat("Range", &m_projectileRange);
                    m_projectileRange = std::max(m_projectileRange, 0.f);
                    tool_tip("How far does the projectile travel?");
                    ImGui::InputFloat("Projectile Radius", &m_projectileRadius);
                    m_projectileRadius = std::max(m_projectileRadius, 0.f);
                    tool_tip(
                        "The projectile has a sphere collider."
                        "\nHow big should the radius of the projectile be?");
                    ImGui::InputFloat("Speed", &m_projectileSpeed);
                    m_projectileSpeed = std::max(m_projectileSpeed, 0.f);
                    tool_tip("How fast does the projectile travel?");
                    break;
            }
        }
    }
}

void bee::AbilityCreationMenu::ApplyTypeSection()
{
    if (ImGui::CollapsingHeader("Apply Type", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Combo("Apply Type.", &m_applyTypesSelectedItem, m_applyTypes, IM_ARRAYSIZE(m_applyTypes));
        tool_tip("What type of ability is it? Instant, effect over time etc.");

        switch (static_cast<ApplyType>(m_applyTypesSelectedItem))
        {
            case ApplyType::Instant:
            {
                if (static_cast<Target>(m_targetsSelectedItem) == Target::Self)
                {
                    ImGui::Combo("Instant Type", &m_instantTypesSelectedItem, m_instantTypes, IM_ARRAYSIZE(m_instantTypes));
                    tool_tip("Effect or dash.");
                }
                break;
            }
            case ApplyType::EffectOverTime:
            {
                ImGui::InputFloat("Tick Duration", &m_duration);
                m_duration = std::max(m_duration, 0.f);
                tool_tip("Effect will apply every X seconds.");
                ImGui::InputInt("Number of Ticks", &m_numberOfTicks);
                m_numberOfTicks = std::max(m_numberOfTicks, 0);
                ImGui::Text("Total Duration: %.2f seconds", m_duration * static_cast<float>(m_numberOfTicks));
                break;
            }
            case ApplyType::BuffDebuff:
            {
                ImGui::InputFloat("Duration", &m_duration);
                tool_tip("After the duration is over the effect will be reverted.");
                m_duration = std::max(m_duration, 0.f);
                break;
            }
            case ApplyType::StatusEffect:
            {
                if (ImGui::BeginCombo("Effect.", m_statusEffectTypes[m_statusEffectTypesSelectedItem]))
                {
                    int numberOfStatusEffects = IM_ARRAYSIZE(m_statusEffectTypes);
                    for (int i = 0; i < numberOfStatusEffects; i++)
                    {
                        const bool isSelected = (m_statusEffectTypesSelectedItem == i);

                        if (ImGui::Selectable(m_statusEffectTypes[i], isSelected))
                        {
                            m_statusEffectTypesSelectedItem = i;
                        }

                        tool_tip(m_statusEffectToolTips[i]);

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::InputFloat("Duration", &m_duration);
                tool_tip("After the duration is over the effect will be reverted.");
                m_duration = std::max(m_duration, 0.f);
                break;
            }
        }
    }
}

void bee::AbilityCreationMenu::DashSection()
{
    if (ImGui::CollapsingHeader("Dash", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputFloat("Speed", &m_dashSpeed);
        tool_tip("How fast the does player travel during the dash?");
        ImGui::InputFloat("Duration", &m_duration);
        tool_tip("How long does the dash last?\nYou will typically want a very small number here.");
    }
}

void bee::AbilityCreationMenu::EffectSection()
{
    if (ImGui::CollapsingHeader("Effect", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Combo("Stat Affected", &m_statsSelectedItem, m_stats, IM_ARRAYSIZE(m_stats));
        tool_tip(
            "What player stat does the ability alter?\n"
            "Ex: Health Decrease in combination with Hostile target deals damage.");

        ImGui::Combo(
            "Increase or Decrease", &m_increaseOrDecreaseSelectedItem, m_increaseOrDecrease,
            IM_ARRAYSIZE(m_increaseOrDecrease));
        tool_tip("Should the ability increase or decrease the selected stat?");

        if (static_cast<IncreaseOrDecrease>(m_increaseOrDecreaseSelectedItem) == IncreaseOrDecrease::Increase)
        {
            if (const auto statsSelectedItem = static_cast<Stat>(m_statsSelectedItem);
                statsSelectedItem != Stat::DealtDamageModifier && statsSelectedItem != Stat::ReceivedDamageReduction)
            {
                ImGui::Checkbox("Clamp to max", &m_clampToMax);
            }
            else
            {
                m_clampToMax = false;
            }
        }

        int appliedAsArraySize = IM_ARRAYSIZE(m_appliedAs);
        if (static_cast<Stat>(m_statsSelectedItem) == Stat::DealtDamageModifier ||
            static_cast<Stat>(m_statsSelectedItem) == Stat::ReceivedDamageReduction)
        {
            // if the selected stat is either protection multiplier or damage multiplier then only allow percentage
            m_appliedAsSelectedItem = static_cast<int>(AppliedAs::Percentage);
            appliedAsArraySize = 1;
        }

        ImGui::Combo("Applied As", &m_appliedAsSelectedItem, m_appliedAs, appliedAsArraySize);
        tool_tip(
            "Flat number or percentage of the base value of the selected stat."
            "\nPercentage should be a number from 1 to 100.");

        if (const auto applyTypesSelectedItem = static_cast<ApplyType>(m_applyTypesSelectedItem);
            applyTypesSelectedItem == ApplyType::Instant || applyTypesSelectedItem == ApplyType::BuffDebuff)
        {
            ImGui::InputFloat("Amount", &m_amount);
            tool_tip("The value that will be applied.");
        }
        else
        {
            ImGui::InputFloat("Amount per Tick", &m_amount);
            tool_tip("The value that will be applied every tick.");
            if (applyTypesSelectedItem == ApplyType::EffectOverTime)
                ImGui::Text(
                    "Total amount applied: %.2f (based on number of ticks)", m_amount * static_cast<float>(m_numberOfTicks));
        }
        m_amount = std::max(m_amount, 0.f);
    }
}

void bee::AbilityCreationMenu::OnSaveClick()
{
    ImGui::PushStyleColor(ImGuiCol_Button, {0.1f, 0.7f, 0.2f, 1.f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.25f, 0.85f, 0.65f, 1.f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.0f, 0.6f, 0.4f, 1.f});
    if (ImGui::Button("Save", {50.f, 20.f}))
    {
        const bool noErrors = CheckForErrors();
        const bool noWarnings = CheckForWarnings();

        if (noErrors)
        {
            if (noWarnings)
            {
                DisplayConfirmOverrideWindowOrSaveOrOverrideAbility();
            }
            else
            {
                if (m_doNotShowWarningsWindowAgain)
                {
                    DisplayConfirmOverrideWindowOrSaveOrOverrideAbility();
                }
                else
                {
                    m_displayWarningsWindow = true;
                }
            }
        }
    }
    ImGui::PopStyleColor(3);
}

void bee::AbilityCreationMenu::DisplayErrors()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.10f, 0.10f, 0.90f);
    for (auto& error : m_errors)
    {
        if (error.second)
        {
            ImGui::Text("%s", error.first.c_str());
        }
    }
    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
}

void bee::AbilityCreationMenu::DisplayEditCombo()
{
    if (ImGui::BeginCombo("Edit", m_editSelectedItem.c_str()))
    {
        const auto& abilities = Engine.ResourceManager().GetAbilityManager().m_abilities;
        for (const auto& [abilityName, abilitySettings] : abilities)
        {
            const bool isSelected = (m_editSelectedItem == abilityName);

            if (ImGui::Selectable(abilityName.c_str(), isSelected))
            {
                m_editSelectedItem = abilityName;
                SetSettingsForEdit(abilityName, abilitySettings);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

bool bee::AbilityCreationMenu::CheckForErrors()
{
    bool noErrors = true;
    m_errors["Give a name!"] = strcmp(m_name, "") == 0;
    const bool applyTypeInstant = static_cast<ApplyType>(m_applyTypesSelectedItem) == ApplyType::Instant;
    m_errors["Duration cannot be zero!"] =
        are_floats_equal(m_duration, 0.f) &&
        (!applyTypeInstant || applyTypeInstant && static_cast<InstantType>(m_instantTypesSelectedItem) == InstantType::Dash);
    m_errors["Number of ticks cannot be zero!"] =
        m_numberOfTicks == 0 && m_applyTypesSelectedItem == static_cast<int>(ApplyType::EffectOverTime);
    m_errors["Cone angle cannot be zero!"] =
        are_floats_equal(m_coneAngle, 0.f) &&
        m_projectileShootTypesSelectedItem == static_cast<int>(ProjectileShootType::Cone) && m_numberOfProjectiles > 1;

    for (const auto& condition : m_errors)
    {
        if (condition.second == true)
        {
            noErrors = false;
            break;
        }
    }
    return noErrors;
}

bool bee::AbilityCreationMenu::CheckForWarnings()
{
    bool noWarnings = true;
    const bool castTypeProjectile = static_cast<CastType>(m_castTypesSelectedItem) == CastType::Projectile;
    const bool applyTypeInstant = static_cast<ApplyType>(m_applyTypesSelectedItem) == ApplyType::Instant;
    m_warnings["Projectile range is zero."] = are_floats_equal(m_projectileRange, 0.f) && castTypeProjectile;
    m_warnings["Projectile radius is zero."] = are_floats_equal(m_projectileRadius, 0.f) && castTypeProjectile;
    m_warnings["Projectile speed is zero."] = are_floats_equal(m_projectileSpeed, 0.f) && castTypeProjectile;
    m_warnings["Space between projectiles is zero."] =
        are_floats_equal(m_spaceBetweenProjectiles, 0.f) && castTypeProjectile &&
        static_cast<ProjectileShootType>(m_projectileShootTypesSelectedItem) == ProjectileShootType::Line;
    m_warnings["AOE radius is zero."] =
        are_floats_equal(m_aoeRadius, 0.f) && static_cast<CastType>(m_castTypesSelectedItem) == CastType::RadiusAroundPlayer;
    m_warnings["Stat effect amount is zero."] =
        are_floats_equal(m_amount, 0.f) &&
        (static_cast<InstantType>(m_instantTypesSelectedItem) == InstantType::Effect && applyTypeInstant ||
         static_cast<ApplyType>(m_applyTypesSelectedItem) != ApplyType::StatusEffect);
    m_warnings["Dash speed is zero."] = are_floats_equal(m_dashSpeed, 0.f) && applyTypeInstant &&
                                        static_cast<InstantType>(m_instantTypesSelectedItem) == InstantType::Dash;

    for (const auto& warning : m_warnings)
    {
        if (warning.second == true)
        {
            noWarnings = false;
            break;
        }
    }
    return noWarnings;
}

bool bee::AbilityCreationMenu::ConfirmOverrideWindow()
{
    bool confirmed = false;
    ImGui::SetNextWindowPos({1920.f / 2.f, 1080.f / 2.f}, ImGuiCond_Once);
    ImGui::Begin("Confirm Override", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Are you sure you want to override this ability: %s?", m_name);
    if (ImGui::Button("Yes"))
    {
        confirmed = true;
        m_displayConfirmOverrideWindow = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("No"))
    {
        confirmed = false;
        m_displayConfirmOverrideWindow = false;
    }
    ImGui::Checkbox("Don't show this window again.", &m_doNotShowConfirmOverrideWindowAgain);
    ImGui::End();
    return confirmed;
}

bool bee::AbilityCreationMenu::WarningsWindow()
{
    bool confirmed = false;
    ImGui::SetNextWindowPos({1920.f / 2.f, 1080.f / 2.f}, ImGuiCond_Once);
    ImGui::Begin("Warnings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Are you sure you want to save this ability\n"
                "considering the following warnings?");

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.5f, 0.10f, 0.90f);
    for (auto& warning : m_warnings)
    {
        if (warning.second)
        {
            ImGui::Text("%s", warning.first.c_str());
        }
    }
    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);

    if (ImGui::Button("Yes"))
    {
        confirmed = true;
        m_displayWarningsWindow = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("No"))
    {
        confirmed = false;
        m_displayWarningsWindow = false;
    }
    ImGui::Checkbox("Don't show this window again.", &m_doNotShowWarningsWindowAgain);
    ImGui::End();
    return confirmed;
}

void bee::AbilityCreationMenu::DisplayConfirmOverrideWindowOrSaveOrOverrideAbility()
{
    if (Engine.ResourceManager().GetAbilityManager().Get(m_name).has_value() && !m_doNotShowConfirmOverrideWindowAgain)
        m_displayConfirmOverrideWindow = true;
    else
        SaveOrOverrideAbility();
}

void bee::AbilityCreationMenu::SaveOrOverrideAbility()
{
    Engine.ResourceManager().GetAbilityManager().Add(
        m_name, AbilitySettings{
                    static_cast<Target>(m_targetsSelectedItem),
                    m_aoeRadius,
                    m_numberOfProjectiles,
                    m_projectileRange,
                    m_projectileRadius,
                    m_projectileSpeed,
                    static_cast<ProjectileShootType>(m_projectileShootTypesSelectedItem),
                    m_spaceBetweenProjectiles,
                    m_coneAngle,
                    static_cast<CastType>(m_castTypesSelectedItem),
                    static_cast<ApplyType>(m_applyTypesSelectedItem),
                    static_cast<InstantType>(m_instantTypesSelectedItem),
                    m_dashSpeed,
                    static_cast<StatusEffectType>(m_statusEffectTypesSelectedItem),
                    static_cast<Stat>(m_statsSelectedItem),
                    m_amount,
                    static_cast<IncreaseOrDecrease>(m_increaseOrDecreaseSelectedItem),
                    m_clampToMax,
                    static_cast<AppliedAs>(m_appliedAsSelectedItem),
                    m_duration,
                    m_numberOfTicks});

    auto view = Engine.ECS().Registry().view<AbilitiesOnPlayerComponent>();
    for (auto [entity, abilitiesComponent] : view.each())
    {
        if (static_cast<int>(abilitiesComponent.abilityNamesToInputsMap.size()) < abilitiesComponent.maxNumberOfAbilities)
        {
            if (auto it = abilitiesComponent.abilityNamesToInputsMap.find(m_name);
                it == abilitiesComponent.abilityNamesToInputsMap.end()) // if the player does not already have the ability
            {
                abilitiesComponent.abilityNamesToInputsMap[m_name] = {}; // nullopt
            }
        }
    }
    m_editSelectedItem = m_name;
}

void bee::AbilityCreationMenu::SetSettingsForEdit(const std::string& name, const AbilitySettings& settings)
{
    strcpy_s(m_name, name.c_str());
    m_targetsSelectedItem = static_cast<int>(settings.targetTeam);
    m_aoeRadius = settings.aoeRadius;
    m_numberOfProjectiles = settings.numberOfProjectiles;
    m_projectileRange = settings.projectileRange;
    m_projectileRadius = settings.projectileRadius;
    m_projectileSpeed = settings.projectileSpeed;
    m_projectileShootTypesSelectedItem = static_cast<int>(settings.shootType);
    m_spaceBetweenProjectiles = settings.spaceBetweenProjectiles;
    m_coneAngle = settings.coneAngle;
    m_castTypesSelectedItem = static_cast<int>(settings.castType);
    m_applyTypesSelectedItem = static_cast<int>(settings.applyType);
    m_instantTypesSelectedItem = static_cast<int>(settings.instantType);
    m_dashSpeed = settings.dashSpeed;
    m_statusEffectTypesSelectedItem = static_cast<int>(settings.statusEffectType);
    m_statsSelectedItem = static_cast<int>(settings.statAffected);
    m_amount = settings.statAffectAmount;
    m_increaseOrDecreaseSelectedItem = static_cast<int>(settings.increaseOrDecrease);
    m_clampToMax = settings.clampToMax;
    m_appliedAsSelectedItem = static_cast<int>(settings.appliedAs);
    m_duration = settings.durationAmount;
    m_numberOfTicks = settings.numberOfTicks;
}
